#include "Game.hpp"

Game::Game(sf::RenderWindow& pWin)
	: win(pWin),
	wallMap(pWin, player),
	pointer(pWin),
	player(wallMap, "res/sprites/player.png", { 67, 48 }, pointer)
{
	loadEditTextures();
}

void Game::update(double dt) {
	if (inEditor) {
		handleEditorUpdate();
		return;
	}
	dt = std::min(dt, 1.0/30.0);
	player.update(dt);
	pointer.update(dt);
	EffectsManager::Instance().update(dt);
	wallMap.update(dt);
}

 void Game::draw(sf::RenderWindow & win) {
	if (closing) return;
	wallMap.draw();
	player.draw(win);
	pointer.draw();
	EffectsManager::Instance().draw(win);
	if (inEditor) handleEditorDraw();
 }

 // ============================================== EDITOR

void Game::im() {
	if (!inEditor) {
		if (ImGui::Button("Editor")) {
			editorSprite.setTexture(editTextures[editMode]);
			inEditor = true;
			player.isGameInEditor = true;
		}
		ImGui::Value("isGrounded", player.isGrounded);
		ImGui::Value("dx", player.dx);
		return;
	}

	if (ImGui::Button("Return Game")) {
		inEditor = false;
		player.isGameInEditor = false;
		editMode = EditMode::Box;
	}

	if (ImGui::Button("Box")) {
		editMode = EditMode::Box;
		editorSprite.setTexture(editTextures[editMode]);
	}

	if (ImGui::Button("Edge1")) {
		editMode = EditMode::Edge1;
		editorSprite.setTexture(editTextures[editMode]);
	}

	if (ImGui::Button("Edge2")) {
		editMode = EditMode::Edge2;
		editorSprite.setTexture(editTextures[editMode]);
	}

	if (ImGui::Button("Angle1")) {
		editMode = EditMode::Angle1;
		editorSprite.setTexture(editTextures[editMode]);
	}

	if (ImGui::Button("Angle2")) {
		editMode = EditMode::Angle2;
		editorSprite.setTexture(editTextures[editMode]);
	}

	if (ImGui::Button("Ground")) {
		editMode = EditMode::Ground;
		editorSprite.setTexture(editTextures[editMode]);
	}

	if (ImGui::Button("Dirt")) {
		editMode = EditMode::Dirt;
		editorSprite.setTexture(editTextures[editMode]);
	}

	if (ImGui::Button("Enemy")) {
		editMode = EditMode::Enemy;
		editorSprite.setTexture(editTextures[editMode]);
	}

	if (ImGui::Button("Player")) {
		editMode = EditMode::Player;
		editorSprite.setTexture(editTextures[editMode]);
	}

	if (!ImGui::GetIO().WantCaptureMouse && ImGui::IsMouseDown(ImGuiMouseButton_Left))//ImGui::IsMouseClicked)
		handleActionFromEditor();
}

void Game::loadEditTextures() {
	editTextures[EditMode::Box].loadFromFile("res/sprites/box.png");
	editTextures[EditMode::Edge1].loadFromFile("res/sprites/edge1.png");
	editTextures[EditMode::Edge2].loadFromFile("res/sprites/edge2.png");
	editTextures[EditMode::Angle1].loadFromFile("res/sprites/angle1.png");
	editTextures[EditMode::Angle2].loadFromFile("res/sprites/angle2.png");
	editTextures[EditMode::Ground].loadFromFile("res/sprites/ground.png");
	editTextures[EditMode::Dirt].loadFromFile("res/sprites/dirt.png");
	editTextures[EditMode::Enemy].loadFromFile("res/sprites/edenemy.png");
	editTextures[EditMode::Player].loadFromFile("res/sprites/edplayer.png");
}

void Game::handleEditorUpdate() {
	mousePosWorld = win.mapPixelToCoords(sf::Mouse::getPosition(win));
	sf::Vector2f posToDraw;

	if (editMode == EditMode::Box
		|| editMode == EditMode::Edge1
		|| editMode == EditMode::Edge2
		|| editMode == EditMode::Angle1
		|| editMode == EditMode::Angle2
		|| editMode == EditMode::Dirt
		|| editMode == EditMode::Ground)
	{
		float cx = ((int)(mousePosWorld.x / C::GRID_SIZE));
		float cy = ((int)(mousePosWorld.y / C::GRID_SIZE));
		float sprtX = cx * C::GRID_SIZE;
		float sprtY = cy * C::GRID_SIZE;
		posToDraw = { sprtX, sprtY };

		editorSprite.setScale({ 1.0f, 1.0f });
		editorSprite.setOrigin({ 0.0f, 0.0f });

		canDrop = !wallMap.isWall(cx, cy);

		if (canDrop)
			for (sf::Vector2i v : player.getVHitboxCells())
				if (v.x == cx && v.y == cy) {
					canDrop = false;
					break;
				}

		if (canDrop)
			for (Enemy& e : wallMap.enemies) {
				for (sf::Vector2i v : e.getVHitboxCells())
					if (v.x == cx && v.y == cy) {
						canDrop = false;
						break;
					}
				if (!canDrop) break;
			}
	}
	else
	{
		posToDraw = mousePosWorld;
		editorSprite.setScale({ 2.0f, 2.0f });
		sf::FloatRect rect;

		if (editMode == EditMode::Enemy) {
			editorSprite.setOrigin({ 21.0f, 21.0f });
			rect = wallMap.enemies[0].vBox;
		}
		else {
			editorSprite.setOrigin({ 33.0f, 24.0f });
			rect = player.vBox;
		}

		std::vector<sf::Vector2i> cellsToCheck = getCellsFromMouseCentered(mousePosWorld, rect.width, rect.height);
		canDrop = true;

		for (sf::Vector2i cell : cellsToCheck)
			if (wallMap.isWall(cell.x, cell.y)) {
				canDrop = false;
				break;
			}

		if (canDrop)
			for (sf::Vector2i cell : cellsToCheck) {
				for (sf::Vector2i pCell : player.getVHitboxCells())
					if (cell.x == pCell.x && cell.y == pCell.y) {
						canDrop = false;
						break;
					}
				if (!canDrop) break;
			}

		if (canDrop)
			for (sf::Vector2i cell : cellsToCheck) {
				for (Enemy& e : wallMap.enemies) {
					for (sf::Vector2i v : e.getVHitboxCells())
						if (cell.x == v.x && cell.y == v.y) {
							canDrop = false;
							break;
						}
					if (!canDrop) break;
				}
				if (!canDrop) break;
			}	
	}

	if (canDrop)
		editorSprite.setColor({ 255, 255, 255 });
	else
		editorSprite.setColor({ 255, 0, 0 });

	editorSprite.setPosition(posToDraw);
}

void Game::handleEditorDraw() {
	win.draw(editorSprite);
}

void Game::handleActionFromEditor() {
	if (!canDrop) return;

	float cx = ((int)(mousePosWorld.x / C::GRID_SIZE));
	float cy = ((int)(mousePosWorld.y / C::GRID_SIZE));

	if (editMode == EditMode::Box)
		wallMap.addWall(WallMap::WallType::Box, cx, cy);
	else if (editMode == EditMode::Edge1)
		wallMap.addWall(WallMap::WallType::Edge1, cx, cy);
	else if (editMode == EditMode::Edge2)
		wallMap.addWall(WallMap::WallType::Edge2, cx, cy);
	else if (editMode == EditMode::Angle1)
		wallMap.addWall(WallMap::WallType::Angle1, cx, cy);
	else if (editMode == EditMode::Angle2)
		wallMap.addWall(WallMap::WallType::Angle2, cx, cy);
	else if (editMode == EditMode::Ground)
		wallMap.addWall(WallMap::WallType::Ground, cx, cy);
	else if (editMode == EditMode::Dirt)
		wallMap.addWall(WallMap::WallType::Dirt, cx, cy);
	else if (editMode == EditMode::Player)
		player.setForEditorInstance(mousePosWorld);
	else if (editMode == EditMode::Enemy)
		wallMap.addEnemy(mousePosWorld, true);
}

// ============================================== OLD

void Game::processInput(sf::Event ev) {
	if (ev.type == sf::Event::Closed) {
		closing = true;
		return;
	}
}

int blendModeIndex(sf::BlendMode bm) {
	if (bm == sf::BlendAlpha) return 0;
	if (bm == sf::BlendAdd) return 1;
	if (bm == sf::BlendNone) return 2;
	if (bm == sf::BlendMultiply) return 3;
	return 4;
};

