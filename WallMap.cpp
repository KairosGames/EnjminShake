#include "WallMap.h"
#include "EffectsManager.h"

WallMap::WallMap(sf::RenderWindow& pWin, Player& pPlayer) : win(pWin), player(pPlayer) {
	camera = win.getView();
	loadBackgrounds();
	loadWallTextures();
	buildMap();
	loadEnnemies();
}

WallMap::Wall::Wall(uint64_t pKey, WallType pType, std::unordered_map<WallType, sf::Texture>& pTextures) : key(pKey), type(pType) {
	sprite.setTexture(pTextures[pType]);
	sf::Vector2i pos = getVec2i(pKey);
	sprite.setPosition((float)pos.x * C::GRID_SIZE, (float)pos.y * C::GRID_SIZE);
}

void WallMap::update(double dt) {
	updateBackgrounds();
	updateCamera(dt);
	for (Enemy& e : enemies) e.update(dt);
	for (int i = bullets.size() - 1; i >= 0; i--) {
		Bullet& b = bullets[i];
		b.update(dt);
		if (b.checkCollision(player, *this, win.getView().getCenter())) bullets.erase(bullets.begin() + i);
	}
}

void WallMap::draw() {
	drawBackgrounds();
	for (Wall& r : walls) win.draw(r.sprite);
	for (Enemy& e : enemies) e.draw(win);
	for (Bullet& b : bullets) b.draw(win);
}

void WallMap::updateCamera(double dt) {
	camera = win.getView();
	sf::Vector2f target = { player.pos.x, C::RES_Y / 2 };
	sf::Vector2f camPos = lerpVec(camera.getCenter(), target, 0.005f, dt);
	if (camPos.x <= C::RES_X / 2) camPos.x = C::RES_X / 2;
	else if (camPos.x >= C::RES_X * 2.5) camPos.x = C::RES_X * 2.5;
	camera.setCenter(camPos);
	updateShake(dt);
	win.setView(camera);
}

void WallMap::updateShake(double dt) {
	if (shakeTimer > 0.0f) {
		shakeTimer -= (float)dt;
		float strength = shakeStrength * (shakeTimer / shakeDuration);
		float offsetX = ((rand() / (float)RAND_MAX) * 2.0f - 1.0f) * strength;
		float offsetY = ((rand() / (float)RAND_MAX) * 2.0f - 1.0f) * strength;
		camera.setCenter(camera.getCenter().x + offsetX, camera.getCenter().y + offsetY);
	}
	else if (camera.getCenter().y != C::RES_Y * 0.5f) {
		camera.setCenter(camera.getCenter().x, C::RES_Y * 0.5f);
	}
}

void WallMap::shakeCamera(float duration, float strength) {
	shakeTimer = duration;
	shakeDuration = duration;
	shakeStrength = strength;
}

void WallMap::loadWallTextures() {
	wallTextures.reserve(3);
	wallTextures[WallType::Ground].loadFromFile("res/sprites/ground.png");
	wallTextures[WallType::Edge1].loadFromFile("res/sprites/edge1.png");
	wallTextures[WallType::Edge2].loadFromFile("res/sprites/edge2.png");
	wallTextures[WallType::Angle1].loadFromFile("res/sprites/angle1.png");
	wallTextures[WallType::Angle2].loadFromFile("res/sprites/angle2.png");
	wallTextures[WallType::Dirt].loadFromFile("res/sprites/dirt.png");
	wallTextures[WallType::Box].loadFromFile("res/sprites/box.png");
}

void WallMap::buildMap() {
	buildLimits();
	addAllBoxes();
}

void WallMap::buildLimits() {
	int cols = C::RES_X / C::GRID_SIZE;
	int lastLine = C::RES_Y / C::GRID_SIZE - 1;
	walls.clear();
	for (int i = 0; i < cols * 4; i++) {

		if (i == 0)
			addWall(Angle1, i, lastLine + 1);
		else if (i == (cols * 3) - 1)
			addWall(Angle2, i, lastLine + 1);
		else {
			if (i == 65 || 66)
				addWall(Dirt, i, lastLine + 1);
			else
				addWall(Ground, i, lastLine + 1);
		}

		addWall(Dirt, i, -1);
		addWall(Dirt, i, lastLine + 2);
	}

	for (int i = lastLine; i >= 0; i--)
		addWall(Edge1, 0, i);

	for (int i = lastLine; i >= 0; i--)
		addWall(Edge2, (cols * 3) - 1, i);
}

void WallMap::addAllBoxes() {
	addWall(Ground, 3, 11);
	addWall(Ground, 4, 11);
	addWall(Ground, 5, 11);
	addWall(Ground, 6, 11);
	addWall(Box, 8, 14);
	addWall(Box, 8, 15);
	addWall(Ground, 9, 9);
	addWall(Ground, 10, 9);
	addWall(Ground, 11, 9);
	addWall(Box, 11, 10);
	addWall(Box, 11, 11);
	addWall(Ground, 11, 12);
	addWall(Ground, 12, 12);
	addWall(Ground, 13, 12);
	addWall(Ground, 14, 12);
	addWall(Ground, 15, 12);
	addWall(Ground, 16, 12);
	addWall(Ground, 17, 12);
	addWall(Ground, 17, 6);
	addWall(Ground, 18, 6);
	addWall(Ground, 19, 6);
	addWall(Box, 20, 14);
	addWall(Box, 20, 5);
	addWall(Ground, 20, 6);
	addWall(Box, 20, 15);
	addWall(Box, 21, 14);
	addWall(Box, 21, 15);
	addWall(Box, 22, 14);
	addWall(Box, 22, 15);
	addWall(Box, 23, 14);
	addWall(Box, 23, 15);
	addWall(Box, 24, 14);
	addWall(Box, 24, 15);
	addWall(Box, 25, 14);
	addWall(Box, 25, 15);
	addWall(Box, 26, 14);
	addWall(Box, 26, 15);
	addWall(Box, 27, 14);
	addWall(Box, 27, 15);
	addWall(Box, 28, 14);
	addWall(Box, 28, 15);
	addWall(Box, 29, 14);
	addWall(Box, 29, 15);
	addWall(Box, 30, 14);
	addWall(Box, 30, 15);
	addWall(Box, 31, 14);
	addWall(Box, 31, 15);
	addWall(Box, 30, 3);
	addWall(Box, 31, 3);
	addWall(Box, 32, 3);
	addWall(Ground, 34, 11);
	addWall(Ground, 35, 11);
	addWall(Ground, 36, 11);
	addWall(Box, 38, 9);
	addWall(Box, 39, 9);
	addWall(Box, 40, 9);
	addWall(Ground, 36, 6);
	addWall(Ground, 35, 6);
	addWall(Ground, 34, 6);
	addWall(Ground, 39, 4);
	addWall(Ground, 40, 4);
	addWall(Ground, 41, 4);
	addWall(Ground, 42, 4);
	addWall(Ground, 43, 4);
	addWall(Ground, 44, 4);
	addWall(Ground, 59, 5);
	addWall(Ground, 58, 5);
	addWall(Ground, 57, 5);
	addWall(Ground, 59, 9);
	addWall(Ground, 58, 9);
	addWall(Ground, 57, 9);
	addWall(Ground, 59, 13);
	addWall(Ground, 58, 13);
	addWall(Ground, 57, 13);
	addWall(Box, 47, 8);
	addWall(Box, 50, 8);
	addWall(Box, 51, 8);
	addWall(Ground, 43, 10);
	addWall(Ground, 44, 10);
	addWall(Ground, 45, 10);
	addWall(Box, 41, 15);
	addWall(Box, 41, 14);
	addWall(Box, 48, 8);
	addWall(Box, 49, 8);
	addWall(Ground, 60, 5);
	addWall(Edge2, 60, 9);
	addWall(Edge2, 60, 13);
	addWall(Edge2, 60, 11);
	addWall(Box, 60, 4);
	addWall(Edge2, 60, 2);
	addWall(Box, 61, 2);
	addWall(Box, 62, 2);
	addWall(Box, 64, 2);
	addWall(Box, 65, 2);
	addWall(Box, 66, 2);
	addWall(Box, 67, 2);
	addWall(Box, 68, 2);
	addWall(Box, 69, 2);
	addWall(Box, 70, 2);
	addWall(Box, 63, 2);
	addWall(Edge2, 60, 3);
	addWall(Edge2, 60, 6);
	addWall(Edge2, 60, 7);
	addWall(Edge2, 60, 8);
	addWall(Edge2, 60, 10);
	addWall(Edge2, 60, 12);
	addWall(Ground, 70, 3);
	addWall(Ground, 71, 3);
	addWall(Ground, 72, 3);
	addWall(Ground, 73, 3);
	addWall(Ground, 74, 3);
	addWall(Box, 75, 3);
	addWall(Box, 75, 4);
	addWall(Box, 76, 4);
	addWall(Box, 77, 4);
	addWall(Box, 78, 4);
	addWall(Box, 79, 4);
	addWall(Box, 80, 4);
	addWall(Edge2, 60, 14);
	addWall(Ground, 61, 14);
	addWall(Ground, 62, 14);
	addWall(Ground, 61, 11);
	addWall(Ground, 62, 11);
	addWall(Ground, 61, 8);
	addWall(Ground, 62, 8);
	addWall(Ground, 61, 5);
	addWall(Ground, 62, 5);
	addWall(Box, 76, 19);
	addWall(Ground, 68, 7);
	addWall(Ground, 69, 7);
	addWall(Ground, 70, 7);
	addWall(Ground, 68, 11);
	addWall(Ground, 69, 11);
	addWall(Ground, 70, 11);
	addWall(Ground, 74, 9);
	addWall(Ground, 76, 9);
	addWall(Ground, 75, 9);
	addWall(Box, 73, 13);
	addWall(Box, 74, 13);
	addWall(Box, 71, 15);
	addWall(Ground, 81, 12);
	addWall(Ground, 82, 12);
	addWall(Ground, 83, 12);
	addWall(Ground, 84, 12);
	addWall(Ground, 65, 15);
	addWall(Ground, 66, 15);
	addWall(Ground, 84, 9);
	addWall(Ground, 85, 9);
	addWall(Ground, 86, 9);
	addWall(Ground, 87, 5);
	addWall(Ground, 86, 5);
	addWall(Ground, 85, 5);
	addWall(Ground, 84, 5);
	addWall(Box, 79, 7);
	addWall(Box, 80, 7);
}

void WallMap::loadEnnemies() {
	addEnemy({ 325.0f, 665.0f });
	addEnemy({920.0f, 730.0f});
	addEnemy({ 920.0f, 990.0f });
	addEnemy({ 1200.0f, 350.0f });
	addEnemy({ 1545.0f, 855.0f });
	addEnemy({ 2010.0f, 155.0f });
	addEnemy({ 3160.0f, 470.0f });
	addEnemy({ 2530.0f, 535.0f });
	addEnemy({ 2270.0f, 665.0f });
	addEnemy({ 2815.0f, 985.0f });
	addEnemy({ 3470.0f, 985.0f });
	addEnemy({ 3740.0f, 795.0f });
	addEnemy({ 3705.0f, 535.0f });
	addEnemy({ 3780.0f, 275.0f });
	addEnemy({ 3905.0f, 45.0f });
	addEnemy({ 4325.0f, 85.0f });
	addEnemy({ 5020.0f, 220.0f });
	addEnemy({ 3945.0f, 285.0f });
	addEnemy({ 3995.0f, 475.0f });
	addEnemy({ 3940.0f, 670.0f });
	addEnemy({ 3985.0f, 860.0f });
	addEnemy({ 4410.0f, 415.0f });
	addEnemy({ 4220.0f, 925.0f });
	addEnemy({ 4445.0f, 665.0f });
	addEnemy({ 5280.0f, 730.0f });
	addEnemy({ 5120.0f, 415.0f });
	addEnemy({ 4725.0f, 990.0f });
	addEnemy({ 5465.0f, 540.0f });
	addEnemy({ 5495.0f, 285.0f });
	addEnemy({ 5155.0f, 900.0f });
	addEnemy({ 4940.0f, 795.0f });
	addEnemy({ 4840.0f, 395.0f });
	addEnemy({ 4675.0f, 25.0f });
}

uint64_t WallMap::getKey(int x, int y) {
	return (uint64_t(uint32_t(x)) << 32) | uint32_t(y);
}

bool WallMap::isWall(int x, int y) {
	return wallIDs.find(getKey(x, y)) != wallIDs.end();
}

void WallMap::addWall(WallType type, int x, int y) {
	uint64_t key = getKey(x, y);
	if (wallIDs.find(key) != wallIDs.end()) return;
	wallIDs.emplace(key, type);
	walls.push_back({ key, type, wallTextures });
}

void WallMap::removeWall(int x, int y) {
	if (!isWall(x, y)) return;
	uint64_t key = getKey(x, y);
	wallIDs.erase(key);
	for (int i = walls.size() - 1; i >= 0; i--)
		if (walls[i].key == key) {
			walls.erase(walls.begin() + i);
			break;
		}
}

sf::Vector2i WallMap::getVec2i(uint64_t key) {
	return { int32_t(key >> 32), int32_t(key & 0xFFFFFFFFu) };
}

WallMap::WallType WallMap::getType(int x, int y) {
	return wallIDs[getKey(x, y)];
}

void WallMap::destroyBox(int x, int y) {
	float effx = (x * C::GRID_SIZE) + C::GRID_SIZE / 2;
	float effy = (y * C::GRID_SIZE) + C::GRID_SIZE / 2;
	EffectsManager::Instance().playAnimEffect(EffectsManager::AnimEffectType::BoxExplosion, { effx, effy });
	removeWall(x, y);
}

void WallMap::loadBackgrounds() {
	for (int i = 1; i <= 8; i++) {
		sf::Texture tex;
		tex.loadFromFile("res/sprites/bg/bg" + std::to_string(i) + ".png");
		bgTex.push_back(tex);
	}

	for (int i = 0; i < bgTex.size(); i++) {
		backgrounds.push_back(std::vector<sf::Sprite>());
		for (int j = 0; j < 3; j++) {
			sf::Sprite spr;
			spr.setTexture(bgTex[i]);
			int n = (j == 0) ? -1 : (j == 1) ? 0 : 1;
			spr.setPosition(n * C::RES_X, 0.0f);
			spr.setOrigin(1.0f, 0.0f);
			spr.setColor(bgTint);
			backgrounds[i].push_back(spr);
		}
	}
}

void WallMap::updateBackgrounds() {
	float v1 = 0.00f;
	float v2 = 0.40f;
	float v3 = 0.20f;
	float v4 = 0.21f;
	float v5 = 0.30f;
	float v6 = v2;
	float v7 = 0.50f;
	float v8 = 0.70f;
	float v9 = 1.00f;

	std::vector<float> speeds = { v1, v2, v3, v4, v5, v6, v7, v8, v9 };

	sf::View v = win.getView();
	float viewLeft = v.getCenter().x - (C::RES_X * 0.5f);

	static bool init = false;
	static float prevViewLeft = 0.0f;
	if (!init) {
		prevViewLeft = viewLeft;
		init = true;
	}

	float camDx = viewLeft - prevViewLeft;
	prevViewLeft = viewLeft;

	float tileW = (float)C::RES_X;
	float viewRight = viewLeft + tileW;

	for (int layer = 0; layer < backgrounds.size() && layer < speeds.size(); layer++) {
		float dx = camDx * speeds[layer];
		for (sf::Sprite& spr : backgrounds[layer])
			spr.move(dx, 0.0f);

		std::vector<sf::Sprite>& tiles = backgrounds[layer];

		auto minmaxX = [&]() {
			float minX = tiles[0].getPosition().x;
			float maxX = minX;
			int minI = 0;
			int maxI = 0;
			for (int i = 1; i < tiles.size(); i++) {
				float x = tiles[i].getPosition().x;
				if (x < minX) { minX = x; minI = i; }
				if (x > maxX) { maxX = x; maxI = i; }
			}
			return std::tuple<float, int, float, int>(minX, minI, maxX, maxI);
			};

		for (int guard = 0; guard < 4; guard++) {
			auto [minX, minI, maxX, maxI] = minmaxX();
			if (viewRight <= maxX + tileW) break;
			tiles[minI].setPosition(maxX + tileW, tiles[minI].getPosition().y);
		}

		for (int guard = 0; guard < 4; guard++) {
			auto [minX, minI, maxX, maxI] = minmaxX();
			if (viewLeft >= minX) break;
			tiles[maxI].setPosition(minX - tileW, tiles[maxI].getPosition().y);
		}
	}
}

void WallMap::drawBackgrounds() {
	for (int i = backgrounds.size() - 1; i >= 0; i--)
		for (int j = 0; j < backgrounds[i].size(); j++)
			win.draw(backgrounds[i][j]);
}

void WallMap::addEnemy(sf::Vector2f pPos, bool isFromEditor) {
	enemies.push_back(Enemy(pPos, *this, player, "res/sprites/enemy.png", { 43, 42 }));
	if (isFromEditor) enemies.back().setForEditorInstance();
}