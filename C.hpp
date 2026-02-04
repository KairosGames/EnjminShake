#pragma once

#include<random>

namespace C {
	static constexpr int GRID_SIZE = 64;
	static constexpr int RES_X = 1920;
	static constexpr int RES_Y = 1080;
	static bool IS_DEBUG = false;
	const float PI = 3.14159265f;
	const float EPS = 0.001f;
}

inline float lerp(float a, float b, float t, double dt) {
	float dtT = t * dt * 10000.0f;
	return a + (b - a) * t;
}

inline sf::Vector2f lerpVec(sf::Vector2f a, sf::Vector2f b, float t, double dt) {
	return { lerp(a.x, b.x, t, dt), lerp(a.y, b.y, t, dt) };
}

inline float degToRad(float deg) {
	return deg * C::PI / 180.0;
}

inline float radToDeg(float rad) {
	return rad * 180.0f / C::PI;
}

inline sf::Vector2f normalize(sf::Vector2f v) {
	float len = std::sqrt(v.x * v.x + v.y * v.y);
	if (len == 0.0f) return { 0.0f, 0.0f };
	return { v.x / len, v.y / len };
}

inline sf::Vector2f getForward(float angle) {
	float rad = degToRad(angle);
	return normalize({ std::cos(rad), std::sin(rad) });
}

inline sf::Vector2f getUp(float angle){
	float rad = degToRad(angle);
	return normalize({ -std::sin(rad), std::cos(rad) });
}

inline float getDistanceSquared(sf::Vector2f a, sf::Vector2f b) {
	const float dx = b.x - a.x;
	const float dy = b.y - a.y;
	return dx * dx + dy * dy;
}

inline std::mt19937& rnd() {
	static std::mt19937 gen{ std::random_device{}() };
	return gen;
}

inline bool randBool() {
	static std::mt19937 gen{ std::random_device{}() };
	static std::bernoulli_distribution coin(0.5);
	return coin(gen);
}

inline int randi(int a, int b) {
	std::uniform_int_distribution<int> dist(a, b);
	return dist(rnd());
}

inline float randf(float a, float b) {
	std::uniform_real_distribution<float> dist(a, b);
	return dist(rnd());
}

inline bool randb(double p = 0.5) {
	std::bernoulli_distribution dist(p);
	return dist(rnd());
}

static std::vector<sf::Vector2i> rectToGridCells(const sf::FloatRect& worldRect)
{
	int x0 = (int)(worldRect.left / C::GRID_SIZE);
	int x1 = (int)((worldRect.left + worldRect.width - C::EPS) / C::GRID_SIZE);
	int y0 = (int)((worldRect.top + C::EPS) / C::GRID_SIZE);
	int y1 = (int)((worldRect.top + worldRect.height) / C::GRID_SIZE);

	std::vector<sf::Vector2i> cells;
	if (x1 < x0 || y1 < y0) return cells;

	cells.reserve((x1 - x0 + 1) * (y1 - y0 + 1));
	for (int cy = y0; cy <= y1; ++cy)
		for (int cx = x0; cx <= x1; ++cx)
			cells.emplace_back(cx, cy);

	return cells;
}

static std::vector<sf::Vector2i> getCellsFromMouseCentered(const sf::Vector2f& mouseWorld, float hitW, float hitH) {
	sf::FloatRect r;
	r.left = mouseWorld.x - hitW * 0.5f;
	r.top = mouseWorld.y - hitH * 0.5f;
	r.width = hitW;
	r.height = hitH;

	int x0 = (int)(r.left / C::GRID_SIZE);
	int x1 = (int)((r.left + r.width - C::EPS) / C::GRID_SIZE);
	int y0 = (int)((r.top + C::EPS) / C::GRID_SIZE);
	int y1 = (int)((r.top + r.height) / C::GRID_SIZE);

	std::vector<sf::Vector2i> cells;
	if (x1 < x0 || y1 < y0) return cells;

	cells.reserve((x1 - x0 + 1) * (y1 - y0 + 1));
	for (int cy = y0; cy <= y1; ++cy)
		for (int cx = x0; cx <= x1; ++cx)
			cells.emplace_back(cx, cy);

	return cells;
}