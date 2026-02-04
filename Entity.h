#pragma once

#include <vector>
#include <cmath>

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

#include "C.hpp"
#include "AnimatedSprite.h"

class WallMap;

class Entity
{
public:
	enum AnimType {
		Idle,
		Run,
		RunBack,
		Jump,
		Fall,
		Death
	};

	AnimatedSprite<AnimType> animSprite;
	WallMap& wallMap;
	sf::Vector2f pos;
	sf::Vector2f scale;
	sf::Vector2f renderSize;
	sf::FloatRect vBox;
	sf::FloatRect hBox;
	float leftPaddingBox;
	float rightPaddingBox;
	float topPaddingBox;
	float bottPaddingBox;
	float dx;
	float dy;
	float dxMax;
	float dxRMax;
	float dyMax;
	float addedX;
	float addedY;
	float speedX;
	float speedY;
	float brX;
	float accY;
	bool isPlayer;
	bool isGrounded;
	bool justJump;

	double dmgEffTime = 0.2;
	double dmgTimer = 0.0;
	int life = 3;
	bool isDead = false;
	bool isTakingDamage;

	Entity(WallMap& pWallMap, const std::string& spritePath, sf::Vector2i frameSize);
	virtual void update(double dt);
	virtual void draw(sf::RenderWindow& win);
	void setPos(float x, float y);
	void setPos(sf::Vector2f pPos);
	void setHitBoxes();
	void handleAddedMovement(double dt);
	void slowAddedMovement(double dt);
	void setMove(double dt);
	void moveX(double dt, bool right, float maxSpeed);
	void stopMoveX(double dt);
	void applyGravity(double dt);
	void handleCollisions(double dt);
	void updateHitBoxes(double dt);
	void checkGround(double dy0);
	void jump();
	int getSense();
	void setSense(float sign);
	void handleAnimations(double dt);
	void applyRecoil(float force, float aimedAngle);
	void applyRecoil(float force, sf::Vector2f dir);
	void handleDmgFeedback(double dt);
	void takeDamage(sf::Vector2f dir);
	void die();

	std::vector<sf::Vector2i> getVHitboxCells();

	//debugs
	sf::RectangleShape vBoxShape;
	sf::RectangleShape hBoxShape;
	void setBoxShapes();
	void drawHitBoxes(sf::RenderWindow& win);
};

