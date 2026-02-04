#pragma once

#include <iostream>
#include <array>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <deque>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics.hpp>

#include "C.hpp"
#include "Enemy.h"

class Player;

class WallMap
{
public:
	enum WallType {
		Ground,
		Edge1,
		Edge2,
		Angle1,
		Angle2,
		Dirt,
		Box
	};

	struct Wall {
		Wall(uint64_t pKey, WallType pType, std::unordered_map<WallType, sf::Texture>& pWallTextures);
		uint64_t key;
		WallType type;
		sf::Sprite sprite;
	};

	sf::RenderWindow& win;
	sf::View camera;
	float shakeDuration;
	float shakeStrength;
	double shakeTimer;

	std::vector<sf::Texture> bgTex;
	std::vector<std::vector<sf::Sprite>> backgrounds;
	sf::Color bgTint = { 220,220,220 };

	std::unordered_map<WallType, sf::Texture> wallTextures{};
	std::unordered_map<uint64_t, WallType> wallIDs;
	std::vector<Wall> walls;

	std::deque<Enemy> enemies{};
	std::deque<Enemy> deadEnemies{};
	std::vector<Bullet> bullets{};
	Player& player;


	WallMap(sf::RenderWindow& pWin, Player& pPlayer);
	void update(double dt);
	void draw();
	void updateCamera(double dt);
	void shakeCamera(float duration, float strength);
	void updateShake(double dt);

	static uint64_t getKey(int x, int y);
	static sf::Vector2i getVec2i(uint64_t key);
	void loadWallTextures();
	void buildMap();
	void buildLimits();
	void addAllBoxes();
	void addWall(WallType type, int x, int y);
	void removeWall(int x, int y);
	bool isWall(int x, int y);
	
	WallType getType(int x, int y);
	void destroyBox(int x, int y);

	void loadBackgrounds();
	void updateBackgrounds();
	void drawBackgrounds();

	void loadEnnemies();
	void addEnemy(sf::Vector2f pPos, bool isFromEditor = false);
};

