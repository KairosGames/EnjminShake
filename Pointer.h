#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>

#include "AnimatedSprite.h"
#include "C.hpp"

class Pointer
{
public:
	enum PointerType {
		CanShoot,
		CannotShoot,
		Cursor
	};

	sf::RenderWindow& win;
	sf::Vector2f worldPos;
	sf::Vector2i winPos;
	//AnimatedSprite<PointerType> animSprite;

	Pointer(sf::RenderWindow& pWin);
	void update(double dt);
	void draw();
};

