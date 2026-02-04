#pragma once

#include<unordered_map>
#include<iostream>
#include <vector>
#include <string>

#include "C.hpp"
#include "SFML/Graphics.hpp"

template<typename E>
class AnimatedSprite
{
public:
    struct Anim {
        std::vector<unsigned int> frames;
        double time = 0.0;
        bool loop = false;
        bool isFinished = false;
        int curr = 0;
        double frameTime;
		double speed = 1.0;
    };
    
	sf::Texture texture;
	sf::Sprite sprite;
    sf::Vector2i frameSize;
    std::unordered_map<E, Anim> animations;
    Anim* current = nullptr;
    double frameTimer = 0.0f;
    int maxTileIndex;

	AnimatedSprite(const std::string& texPath, sf::Vector2i pFrameSize);
	AnimatedSprite(const sf::Texture& tex, sf::Vector2i pFrameSize);
	AnimatedSprite(const AnimatedSprite& other);
    void update(double dt);
    void draw(sf::RenderWindow* win);
    void addAnim(E id, const std::vector<unsigned int>& frames, double time, bool loop = false);
    void playAnim(E id);
    void selectFrame(int frame);
    sf::Vector2i getFramePos(int frame);
};

//cpp

template<typename E>
AnimatedSprite<E>::AnimatedSprite(const std::string& texPath, sf::Vector2i pFrameSize) {
	frameSize = pFrameSize;
	if (!texture.loadFromFile(texPath)) std::cout << "SPRITE LOAD ERROR, path : " << texPath << std::endl;
	sprite.setTexture(texture);
	sprite.setTextureRect(sf::IntRect(0, 0, frameSize.x, frameSize.y));
	sprite.setOrigin(pFrameSize.x * 0.5f, pFrameSize.y * 0.5f);
	maxTileIndex = ((texture.getSize().x / frameSize.x) * (texture.getSize().y / frameSize.y)) - 1;
}

template<typename E>
AnimatedSprite<E>::AnimatedSprite(const sf::Texture& tex, sf::Vector2i pFrameSize) {
	frameSize = pFrameSize;
	texture = tex;
	sprite.setTexture(tex);
	sprite.setTextureRect(sf::IntRect(0, 0, frameSize.x, frameSize.y));
	sprite.setOrigin(pFrameSize.x * 0.5f, pFrameSize.y * 0.5f);
	maxTileIndex = ((texture.getSize().x / frameSize.x) * (texture.getSize().y / frameSize.y)) - 1;
}

template<typename E>
AnimatedSprite<E>::AnimatedSprite(const AnimatedSprite& other)
	: texture(other.texture),
	sprite(other.sprite),
	frameSize(other.frameSize),
	animations(other.animations),
	maxTileIndex(other.maxTileIndex)
{
	sprite.setTexture(texture);
}

template<typename E>
void AnimatedSprite<E>::update(double dt) {
	if (current == nullptr || current->isFinished) return;

	frameTimer += dt * current->speed;
	if (frameTimer >= current->frameTime) {
		frameTimer = 0.0f;
		current->curr++;
		if (current->curr > current->frames.size() - 1) {
			if (current->loop) current->curr = 0;
			else { 
				current->isFinished = true;
				current->curr = current->frames.size() - 1;
			}
		}
		selectFrame(current->frames[current->curr]);
	}
}

template<typename E>
void AnimatedSprite<E>::draw(sf::RenderWindow* win) {
	win->draw(sprite);
}

template<typename E>
void AnimatedSprite<E>::addAnim(E id, const std::vector<unsigned int>& frames, double time, bool loop) {
	bool isOk = true;
	for (unsigned int i : frames) if (i > maxTileIndex) isOk = false;
	if (maxTileIndex <= 0 || texture.getSize().x  == 0 || texture.getSize().y == 0) isOk = false;
	if (!isOk) { std::cout << "ANIMATION INDEX ERROR" << std::endl; return; }
	animations[id] = Anim{ frames, time, loop };
	animations[id].frameTime = time / frames.size();
}

template<typename E>
void AnimatedSprite<E>::playAnim(E id) {
	if (current == &animations[id]) return;
	if (current != nullptr) {
		current->curr = 0;
		current->isFinished = false;
	}
	current = &animations[id];
	current->curr = 0;
	selectFrame(current->frames[current->curr]);
	frameTimer = 0.0f;
}

template<typename E>
void AnimatedSprite<E>::selectFrame(int frame) {
	sf::Vector2i frmPos = getFramePos(frame);
	sprite.setTextureRect(sf::IntRect(frmPos.x, frmPos.y, frameSize.x, frameSize.y));
}

template<typename E>
sf::Vector2i AnimatedSprite<E>::getFramePos(int frame) {
	int framesPerLine = texture.getSize().x / frameSize.x;
	int u = (frame % framesPerLine) * frameSize.x;
	int v = (frame / framesPerLine) * frameSize.y;
	return sf::Vector2i(u, v);
}

