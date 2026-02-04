#include "Player.h"


Player::Player(WallMap& pWallMap, const std::string& spritePath, sf::Vector2i frameSize, Pointer& pPointer)
	: Entity(pWallMap, spritePath, frameSize),
	pointer(pPointer),
	weapon(*this, "res/sprites/ak47.png", { 74, 32 })
{
	isPlayer = true;
	dxMax = 500.0f;
	dxRMax = 350.0f;
	brX = 3.0f;
	scale = { 2.0f, 2.0f };
	animSprite.sprite.setScale(scale);
	renderSize = { animSprite.frameSize.x * scale.x , animSprite.frameSize.y * scale.y };
	leftPaddingBox = 23 * scale.x;
	rightPaddingBox = 25 * scale.x;
	topPaddingBox = 14 * scale.y;
	bottPaddingBox = 6 * scale.y;
	pos = { 200.0f, 980.0f };
	setPos(pos);
	setHitBoxes();
	loadAnimations();
	if (C::IS_DEBUG) setBoxShapes();
}

void Player::update(double dt) {
	getInputs(dt);
	if (!isDead) {
		updateSense();
		weapon.update(dt);
	}
	Entity::update(dt);
}

void Player::draw(sf::RenderWindow& win) {
	Entity::draw(win);
	if (!isDead) weapon.draw(win);
}

void Player::getInputs(double dt) {

	if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
		stopMoveX(dt);

	if (isDead) return;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
		moveX(dt, false, dxMax);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
		moveX(dt, true, dxMax);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
		onSpacePressed();
	else
		wasSpacePressed = false;

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		weapon.shoot(dt);
}

void Player::onSpacePressed() {
	if (wasSpacePressed) return;
	jump();
	wasSpacePressed = true;
}

void Player::loadAnimations() {
	animSprite.addAnim(Idle, std::vector<unsigned int>{ 10, 11, 12, 13, 14 }, 0.5, true);
	animSprite.addAnim(Run, std::vector<unsigned int>{ 18, 19, 20, 21, 22, 23, 24, 25 }, 0.45, true);
	animSprite.addAnim(RunBack, std::vector<unsigned int>{ 25, 24, 23, 22, 21, 20, 19, 18 }, 0.8, true);
	animSprite.addAnim(Jump, std::vector<unsigned int>{ 15, 16, 17 }, 0.25, true);
	animSprite.addAnim(Fall, std::vector<unsigned int>{ 7, 8, 9 }, 0.15, true);
	animSprite.addAnim(Death, std::vector<unsigned int>{ 0, 1, 2, 3, 4, 5, 6 }, 1.0, false);
}

void Player::updateSense() {
	float sign = pointer.worldPos.x - pos.x;
	setSense(sign);
}

void Player::setForEditorInstance(sf::Vector2f pPos) {
	setPos(pPos);
	setHitBoxes();
	weapon.targetPos = pPos + weapon.offset;
	weapon.pos = weapon.targetPos;
	weapon.animSprite.sprite.setPosition(weapon.targetPos);
}