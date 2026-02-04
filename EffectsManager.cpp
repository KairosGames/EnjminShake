#include "EffectsManager.h"

EffectsManager::EffectsManager() {
	loadTextures();
	loadAnimations();
}

void EffectsManager::update(double dt) {
	if (animEffToPlay.empty()) return;
	for (int i = (int)animEffToPlay.size() - 1; i >= 0; i--) {
		PlayingEffect& data = animEffToPlay[i];
		AnimEffect& eff = animEffects[data.type][data.index];

		if (eff.isFinished) {
			eff.isPlaying = false;
			animEffToPlay.erase(animEffToPlay.begin() + i);
			continue;
		}

		eff.timer += dt * eff.speed;
		if (eff.timer >= eff.frameTime) {
			eff.timer = 0.0;
			eff.curr++;
			if (eff.curr > eff. maxTileIndex) {
				eff.isPlaying = false;
				eff.isFinished = true;
				animEffToPlay.erase(animEffToPlay.begin() + i);
				continue;
			}
			int framesPerLines = eff.sprite.getTexture()->getSize().x / eff.frameSize.x;
			int u = (eff.curr % framesPerLines) * eff.frameSize.x;
			int v = (eff.curr / framesPerLines) * eff.frameSize.y;
			eff.sprite.setTextureRect(sf::IntRect(u, v, eff.frameSize.x, eff.frameSize.y));
		}
	}
}

void EffectsManager::draw(sf::RenderWindow& win) {
	for (const PlayingEffect& data : animEffToPlay)
		win.draw(animEffects[data.type][data.index].sprite);
}

void EffectsManager::loadTextures() {
	textures[AnimEffectType::FireMuzzle].loadFromFile("res/sprites/fire_muzzle.png");
	textures[AnimEffectType::Explosion].loadFromFile("res/sprites/hit.png");
	textures[AnimEffectType::BoxExplosion].loadFromFile("res/sprites/box_explosion.png");
}

void EffectsManager::loadAnimations() {
	animEffects[AnimEffectType::FireMuzzle].push_back(AnimEffect(textures[FireMuzzle], { 12, 7 }, 0.05));
	animEffects[AnimEffectType::Explosion].push_back(AnimEffect(textures[Explosion], { 46, 49 }, 0.2));
	animEffects[AnimEffectType::BoxExplosion].push_back(AnimEffect(textures[BoxExplosion], { 200, 200 }, 0.2));
}

void EffectsManager::playAnimEffect(AnimEffectType type, sf::Vector2f pos, float rot, sf::Vector2f scale) {
	bool hasFound = false;
	for (int i = 0; i < (int)animEffects[type].size(); i++) {
		AnimEffect& eff = animEffects[type][i];
		if (!eff.isPlaying) {
			eff.isPlaying = true;
			eff.isFinished = false;
			eff.curr = 0;
			eff.timer = 0.0;
			eff.sprite.setPosition(pos);
			eff.sprite.setRotation(rot);
			eff.sprite.setScale(scale);
			animEffToPlay.push_back({ type, i });
			hasFound = true;
			break;
		}
	}
	if (hasFound) return;
	AnimEffect data = animEffects[type][0];
	AnimEffect newEff = AnimEffect(textures[type], data.frameSize, data.time);
	animEffects[type].push_back(newEff);
	newEff.sprite.setPosition(pos);
	newEff.sprite.setRotation(rot);
	newEff.sprite.setScale(scale);
	newEff.isPlaying = true;
	animEffToPlay.push_back({ type, (int)animEffects[type].size() - 1 });
}