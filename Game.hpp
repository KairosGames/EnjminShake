#pragma once

#include <vector>
#include <imgui.h>
#include <array>
#include <iostream>
#include <algorithm>

#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"
#include "SFML/Window.hpp"

#include "sys.hpp"
#include "C.hpp"
#include "HotReloadShader.hpp"

#include "Player.h"
#include "WallMap.h"
#include "Pointer.h"
#include "EffectsManager.h"

class HotReloadShader;

class Game {
public:
	enum class EditMode {
		Box,
		Edge1,
		Edge2,
		Angle1,
		Angle2,
		Ground,
		Dirt,
		Enemy,
		Player
	};

	bool closing = false;
	void processInput(sf::Event ev);

	sf::RenderWindow& win;
	WallMap wallMap;
	Pointer pointer;
	Player player;

	bool inEditor;
	bool canDrop;
	EditMode editMode;
	std::unordered_map<EditMode, sf::Texture> editTextures{};
	sf::Sprite editorSprite;
	sf::Vector2f mousePosWorld;


	Game(sf::RenderWindow& win);
	void update(double dt);
	void draw(sf::RenderWindow& win);

	void im();
	void loadEditTextures();
	void handleEditorUpdate();
	void handleEditorDraw();
	void handleActionFromEditor();
};