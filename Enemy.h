#pragma once
#include <random>
#include <utility>

#include "Player.h"
#include"EffectsManager.h"

class Enemy : public Entity
{
public:
	enum ActionState {
		Patrol,
		Chase,
		Attack,
		Dead
	};

	enum WeaponAnimType {
		Wait,
		Shoot
	};

	Player& player;
	sf::Sprite alertSprite;
	ActionState currentState;
	sf::Vector2f startPos;
	sf::Vector2f lookDownOffset;
	sf::Vector2f lookDownPos;
	float viewDistance = 800.0f;
	float patrolDistance = 600.0f;
	float patrolSpeed;
	bool hasSeenPlayer = false;
	bool isGoingRight;

	double memoryTimer = 0.0;
	double memoryTime = 2.0;
	int lastMemDir;

	AnimatedSprite<WeaponAnimType> weapon;
	sf::Vector2f weapPos;
	sf::Vector2f weapScale;
	sf::Vector2f weapOrigin;
	sf::Vector2f weapOffset;
	sf::Vector2f targetPos;
	sf::Texture bulletTex;
	double reloadTime = 0.5f;
	double reloadTimer = 0.0f;
	float aimedAngle = 0.0f;
	float aimRotCont = 0.0f;
	bool aimRotInit;
	int lastAimSense;
	bool canShoot = false;
	bool hasJustJump = false;

	Enemy(sf::Vector2f pPos, WallMap& pWallMap, Player& pPlayer, const std::string& spritePath, sf::Vector2i frameSize);
	void update(double dt) override;
	void draw(sf::RenderWindow& win) override;
	void loadAnimations();
	void updateState();
	void doAction(double dt);
	void updateSense();
	void setForEditorInstance();
	void updateAlertPos();

	void updateWeapon(double dt);
	void updateWeaponPosition(double dt);
	void updateWeaponSense(double dt);
	void updateLookDownPos();
	void aim(double dt);
	void updateWeaponAnimations(double dt);
	void shoot(double dt);
	void updateShootTimer(double dt);
	sf::Vector2f getFirePos();
	void playShootEffect(sf::Vector2f firePos);
	void generateBullet(sf::Vector2f pPos);
	void patrol(double dt);
	bool isThereGround();
	bool canGoForward();
	bool canSeePlayer();
	void handleChase(double dt);
	void handleJump(double dt);
	bool canJump();
};

