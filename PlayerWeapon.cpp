#include "PlayerWeapon.h"
#include "Player.h"
#include "WallMap.h"

PlayerWeapon::PlayerWeapon(Player& pPlayer, const std::string& pTexPath, sf::Vector2i pFrameSize)
	: player (pPlayer),
	texPath(pTexPath),
	animSprite(pTexPath, pFrameSize)
{
	offset = { 0.0f, 18.0f };
	scale = { 1.2f, 1.2f };
	origin = { 27.0f, 15.0f };
	animSprite.sprite.setScale(scale);
	animSprite.sprite.setOrigin(origin);
	pos = { 200.0f, 980.0f };
	targetPos = pos;
	animSprite.sprite.setPosition(pos);
	loadAnimations();
	bulletTex.loadFromFile("res/sprites/bullet.png");
	laser.setFillColor({ 255, 0, 0, 80 });
	laser.setOrigin(0.f, thickness * 0.5f);
}

void PlayerWeapon::update(double dt) {
	updatePosition(dt);
	updateSense();
	updateShootTimer(dt);
	updateAnimations(dt);
	updateLaser();
	animSprite.update(dt);
}

void PlayerWeapon::draw(sf::RenderWindow& win) {
	if (!player.isGameInEditor) drawLaser(win);
	animSprite.draw(&win);
	for (Bullet& b : bullets) win.draw(b.sprite);
}

void PlayerWeapon::loadAnimations() {
	animSprite.addAnim(Wait, std::vector<unsigned int>{ 0, 1, 2, 3, 4 }, 0.5, true);
	animSprite.addAnim(Shoot, std::vector<unsigned int>{ 5, 6, 7, 8, 9, 10, 11, 12 }, 0.1, false);
}

void PlayerWeapon::updatePosition(double dt) {
	float offsetX = offset.x * getSense();
	targetPos = { player.pos.x + offsetX, player.pos.y + offset.y };
	pos = lerpVec(pos, targetPos, 0.1f, dt);
	animSprite.sprite.setPosition(pos);
}

void PlayerWeapon::updateSense() {
	setSense(player.getSense());
	sf::Vector2f target = player.pointer.worldPos;
	sf::Vector2f src = animSprite.sprite.getPosition();
	sf::Vector2f d{ target.x - src.x, target.y - src.y };
	if (d.x == 0.0f && d.y == 0.0f) return;
	aimedAngle = std::atan2(d.y, d.x) * 180.0f / C::PI;
	float spriteAngle = (getSense() > 0) ? aimedAngle : (180.0f + aimedAngle);
	animSprite.sprite.setRotation(spriteAngle);
}

int PlayerWeapon::getSense() {
	return (int)std::copysign(1.0f, animSprite.sprite.getScale().x);
}

void PlayerWeapon::setSense(float sign) {
	animSprite.sprite.setScale(sign * scale.x, scale.y);
}

void PlayerWeapon::updateAnimations(double dt) {
	if (animSprite.current == nullptr)
		animSprite.playAnim(Wait);
	else if (animSprite.current == &animSprite.animations[Shoot] && animSprite.current->isFinished)
		animSprite.playAnim(Wait);
	
	syncIdleAnim();
}

void PlayerWeapon::syncIdleAnim() {
	bool isPlayerIdle = (player.animSprite.current == &player.animSprite.animations[Player::AnimType::Idle]);
	if (!isPlayerIdle && !hasPlayerSwitchFromIdle) {
		hasPlayerSwitchFromIdle = true;
		return;
	}

	if (animSprite.current == &animSprite.animations[Wait] && isPlayerIdle && (hasPlayerSwitchFromIdle || hasWeaponSwitchFromWait)) {
		animSprite.current->curr = player.animSprite.current->curr;
		animSprite.frameTimer = player.animSprite.frameTimer;
		hasPlayerSwitchFromIdle = false;
		hasWeaponSwitchFromWait = false;
	}
}

void PlayerWeapon::updateShootTimer(double dt) {
	if (canShoot) return;
	reloadTimer += dt;
	if (reloadTimer >= reloadTime) {
		reloadTimer = 0.0f;
		canShoot = true;
	}
}

void PlayerWeapon::shoot(double dt) {
	if (!canShoot) return;
	sf::Vector2f firePos = getFirePos();
	animSprite.playAnim(Shoot);
	hasWeaponSwitchFromWait = true;
	generateBullet(firePos);
	playShootEffect(firePos);
	player.applyRecoil(1500.0f, aimedAngle);
	player.wallMap.shakeCamera(0.05f, 2.0f);
	canShoot = false;
}

void PlayerWeapon::playShootEffect(sf::Vector2f firePos) {
	EffectsManager::Instance().playAnimEffect(EffectsManager::AnimEffectType::FireMuzzle, firePos, aimedAngle, {4.0f, 4.0f});
}

sf::Vector2f PlayerWeapon::getFirePos() {
	float frwdOffset = 65.0f;
	float upOffset = -2.0f * player.getSense();
	sf::Vector2f firePos = animSprite.sprite.getPosition() + (getForward(aimedAngle) * frwdOffset) + (getUp(aimedAngle) * upOffset);
	return firePos;
}

void PlayerWeapon::generateBullet(sf::Vector2f firePos) {
	float rAngle = aimedAngle + randf(-4.0f, 4.0f);
	Bullet newBullet(bulletTex, firePos, rAngle);
	player.wallMap.bullets.push_back(newBullet);
}

void PlayerWeapon::updateLaser() {
	frameCounter++;
	if (frameCounter >= framesPerUpdate) {
		frameCounter = 0;
		int ofstArr[]{ 0, -1, -2, -1, 0 };
		float ofst = (animSprite.current == &animSprite.animations[Wait]) ? ofstArr[animSprite.current->curr] : 0;
		sf::Vector2f lasPos = getFirePos() - ((getForward(aimedAngle) * 20.0f) + ((getUp(aimedAngle) * ofst * (float)getSense())));
		laser.setSize({ getLaserLen(lasPos, aimedAngle, 2000.0f), thickness });
		laser.setRotation(aimedAngle);
		laser.setPosition(lasPos);
	}
}

void PlayerWeapon::drawLaser(sf::RenderWindow& win) {
	win.draw(laser);
}

float PlayerWeapon::getLaserLen(sf::Vector2f& start, float angleDeg, float maxLen)
{
	sf::Vector2f dir = getForward(angleDeg);
	float step = 20.0f;
	float dist = 0.0f;

	while (dist < maxLen)
	{
		sf::Vector2f actualPos = start + (dir * dist);
		int cx = int(actualPos.x / C::GRID_SIZE);
		int cy = int(actualPos.y / C::GRID_SIZE);
		if (player.wallMap.isWall(cx,cy))                 
			return dist;

		for (Enemy& e : player.wallMap.enemies)
			if (!e.isDead && e.vBox.contains(actualPos))
				return dist;

		dist += step;
	}
	return maxLen;
}