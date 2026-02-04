#pragma once

#include <vector>
#include <map>
#include <string>
#include <iostream>

#include <SFML/Graphics.hpp>

class EffectsManager
{
public:
    enum AnimEffectType {
        FireMuzzle,
        Explosion,
        BoxExplosion
	};

    struct PlayingEffect {
        AnimEffectType type;
        int index;
    };

    struct AnimEffect {
        sf::Sprite sprite;
		sf::Vector2i frameSize;
        double time = 0.0;
		int maxTileIndex;
        int curr = 0;
        double frameTime;
        double speed = 1.0;
        double timer = 0.0;
        bool isPlaying = false;
        bool isFinished = false;

        AnimEffect(sf::Texture& texture, sf::Vector2i pFrameSize, double pTime) {
			frameSize = pFrameSize;
			time = pTime;
            sprite.setTexture(texture);
            sprite.setTextureRect(sf::IntRect(0, 0, frameSize.x, frameSize.y));
            sprite.setOrigin(pFrameSize.x * 0.5f, pFrameSize.y * 0.5f);
            maxTileIndex = ((texture.getSize().x / frameSize.x) * (texture.getSize().y / frameSize.y)) - 1;
            frameTime = time / (maxTileIndex + 1);
        }
    };

	std::map<AnimEffectType, sf::Texture> textures;
	std::map<AnimEffectType, std::vector<AnimEffect>> animEffects;
    std::vector<PlayingEffect> animEffToPlay;
    
    sf::Texture alertTex;

	EffectsManager();
    static EffectsManager& Instance() {
        static EffectsManager inst;
        return inst;
    }

    void update(double dt);
	void draw(sf::RenderWindow& win);
	void loadTextures();
    void loadAnimations();
	void playAnimEffect(AnimEffectType type, sf::Vector2f pos, float rot = 0.0f, sf::Vector2f scale = { 1.0f, 1.0f });
};

