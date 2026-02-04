#pragma once

#include<algorithm>

#include "SFML/Graphics.hpp"

#include "C.hpp"
#include "EffectsManager.h"

class Player;
class WallMap;

class Bullet
{
public:
	sf::Sprite sprite;
	sf::Vector2f pos;
	sf::Vector2f direction;
	float speed;
	bool isToDelete;

	Bullet(sf::Texture& texture, sf::Vector2f startPos, float pAngle);
	void update(double dt);
	void draw(sf::RenderWindow& win);
	bool checkCollision(Player& player, WallMap& wallMap, sf::Vector2f viewCenter);
	void handleCollision(Player& player, WallMap& wallMap);
	void playExplosion();
};

