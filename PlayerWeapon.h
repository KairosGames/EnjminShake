#pragma once
#include <string>

#include <SFML/Graphics.hpp>

#include "AnimatedSprite.h"
#include"EffectsManager.h"
#include "Bullet.h"

class Player;

class PlayerWeapon
{
public:
	enum AnimType {
		Wait,
		Shoot
	};

	std::string texPath;
	AnimatedSprite<AnimType> animSprite;
	Player& player;
	sf::Vector2f scale;
	sf::Vector2f origin;
	sf::Vector2f pos;
	sf::Vector2f offset;
	sf::Vector2f targetPos;
	float aimedAngle = 0.0f;
	float reloadTime = 0.1f;
	float reloadTimer = 0.0f;
	bool canShoot = true;
	bool hasPlayerSwitchFromIdle = true;
	bool hasWeaponSwitchFromWait = false;

	sf::Texture bulletTex;
	std::vector<Bullet> bullets;

	sf::RectangleShape laser;
	float thickness = 3.0f;
	int framesPerUpdate = 5;
	int frameCounter = 0;

	PlayerWeapon(Player& pPlayer, const std::string& pTexPath, sf::Vector2i pFrameSize);
	void update(double dt);
	void draw(sf::RenderWindow& win);
	void loadAnimations();
	void updatePosition(double dt);
	void updateAnimations(double dt);
	void syncIdleAnim();
	void updateSense();
	void setSense(float sign);
	int getSense();
	void updateShootTimer(double dt);
	void shoot(double dt);
	sf::Vector2f getFirePos();
	void playShootEffect(sf::Vector2f firePos);
	void generateBullet(sf::Vector2f firePos);
	void updateLaser();
	void drawLaser(sf::RenderWindow& win);
	float getLaserLen(sf::Vector2f& start, float angleDeg, float maxLen);
};

