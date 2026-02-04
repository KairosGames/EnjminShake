#include "Bullet.h"
#include "Player.h"
#include "WallMap.h"

Bullet::Bullet(sf::Texture& texture, sf::Vector2f startPos, float pAngle) {
	sprite.setTexture(texture);
	sprite.setRotation(pAngle);
	sprite.setScale(1.0f, 2.0f);
	sprite.setOrigin(texture.getSize().x * 0.5f, texture.getSize().y * 0.5f);
	pos = startPos;
	direction = sf::Vector2f{ std::cos(pAngle * C::PI / 180.0f), std::sin(pAngle * C::PI / 180.0f) };
	speed = 1600.0f;
}

void Bullet::update(double dt) {
	if (isToDelete) return;
	pos.x += direction.x * speed * (float)dt;
	pos.y += direction.y * speed * (float)dt;
	sprite.setPosition(pos);
}

void Bullet::draw(sf::RenderWindow& win) {
	win.draw(sprite);
}

bool Bullet::checkCollision(Player& player, WallMap& wallMap, sf::Vector2f viewCenter) {
	if (isToDelete) return isToDelete;
	bool isOutOfView = (pos.x > viewCenter.x + C::RES_X / 2) || (pos.x < viewCenter.x - C::RES_X / 2) || (pos.y < 0.0f);
	if (isOutOfView) return isToDelete = true;
	handleCollision(player, wallMap);
	return isToDelete;
}

void Bullet::handleCollision(Player& player, WallMap& wallMap) {
	float px = pos.x / C::GRID_SIZE;
	float py = pos.y / C::GRID_SIZE;

	if (wallMap.isWall(px, py)) {
		isToDelete = true;
		playExplosion();
		if (wallMap.getType(px, py) == WallMap::WallType::Box) wallMap.destroyBox(px, py);
		return;
	}

	if (getDistanceSquared(pos, player.pos) < 100.0f * 100.0f && !player.isDead) {
		sf::Rect r = player.vBox;
		float dx = std::max({ r.left - pos.x, 0.f, pos.x - (r.left + r.width) });
		float dy = std::max({ r.top - pos.y, 0.f, pos.y - (r.top + r.height) });
		if (((dx * dx) + (dy * dy)) < 16.0f) {
			isToDelete = true;
			player.takeDamage(direction);
			playExplosion();
			return;
		}
	}

	for (Enemy& e : wallMap.enemies) {
		if (getDistanceSquared(pos, e.pos) >= 100.0f * 100.0f || e.isDead) continue;
		sf::Rect r = e.vBox;
		float dx = std::max({ r.left - pos.x, 0.f, pos.x - (r.left + r.width) });
		float dy = std::max({ r.top - pos.y, 0.f, pos.y - (r.top + r.height) });
		if (((dx * dx) + (dy * dy)) < 16.0f) {
			isToDelete = true;
			e.takeDamage(direction);
			playExplosion();
			e.hasSeenPlayer = true;
			return;
		}
	}
}

void Bullet::playExplosion() {
	float rot = sprite.getRotation() + 45.0f;
	float scl = randf(0.7f, 1.8f);
	EffectsManager::Instance().playAnimEffect(EffectsManager::AnimEffectType::Explosion, pos + (direction * 10.0f), rot, { scl, scl });
}