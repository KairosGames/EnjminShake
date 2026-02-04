#include "Entity.h"
#include "WallMap.h"

Entity::Entity(WallMap& pWallMap, const std::string& spritePath, sf::Vector2i frameSize)
	: animSprite(spritePath, frameSize),
	wallMap(pWallMap)
{
	pos = sf::Vector2f{};
	dyMax = 1000.0f;
	accY = 1000.0f;
	dx = 0.0f;
	dy = 0.0f;
	speedX = 0.0f;
	speedY = 0.0f;
	addedX = 0.0f;
	addedY = 0.0f;
}

void Entity::update(double dt) {
	if (!isGrounded)
		applyGravity(dt);
	handleAddedMovement(dt);
	handleCollisions(dt);
	setMove(dt);
	handleAnimations(dt);
	slowAddedMovement(dt);
	handleDmgFeedback(dt);
}

void Entity::draw(sf::RenderWindow& win) {
	animSprite.draw(&win);
	if (C::IS_DEBUG) drawHitBoxes(win);
}

void Entity::setPos(float x, float y) {
	pos = { x, y };
	setPos(pos);
}

void Entity::setPos(sf::Vector2f pPos) {
	pos = pPos;
	animSprite.sprite.setPosition(pPos);
}

void Entity::setHitBoxes() {
	sf::Vector2f orgn = animSprite.sprite.getOrigin();
	sf::Vector2f scaledOrgn = { orgn.x * scale.x, orgn.y * scale.y };
	vBox = { pos.x - scaledOrgn.x + leftPaddingBox, pos.y - scaledOrgn.y + topPaddingBox, renderSize.x - (leftPaddingBox + rightPaddingBox), renderSize.y - (topPaddingBox + bottPaddingBox) };
	hBox = { pos.x - scaledOrgn.x + leftPaddingBox, pos.y - scaledOrgn.y + topPaddingBox, renderSize.x - (leftPaddingBox + rightPaddingBox), renderSize.y - (topPaddingBox + bottPaddingBox) };
}

void Entity::moveX(double dt, bool right, float maxSpeed) {
	float dir = (right) ? 1.0f : -1.0f;
	float dxS = (dx == 0) ? 0.0f : ((dx > 0) ? 1.0f : -1.0f);
	float max = (getSense() == 1) ? ((right) ? maxSpeed : dxRMax) : ((right) ? dxRMax : maxSpeed);
	dx += dir * max * 2.0f * dt;
	if (dxS != 0 && dxS != dir)
		dx += dir * maxSpeed * brX * dt;
	if (std::abs(dx) >= max)
		dx = max * dxS;
}

void Entity::handleAddedMovement(double dt) {
	speedX = dx + addedX;
	speedY = dy + addedY;
}

void Entity::slowAddedMovement(double dt) {
	addedX *= 0.8f;
	addedY *= 0.8f;
	if (addedX < 0.1f && addedX > -0.1f) addedX = 0.0f;
	if (addedY < 0.1f && addedY > -0.1f) addedY = 0.0f;
}

void Entity::setMove(double dt) {
	pos.x += speedX * dt;
	pos.y += speedY * dt;
	animSprite.sprite.setPosition(pos.x, pos.y);
}

void Entity::stopMoveX(double dt) {
	float dxS = (dx == 0) ? 0.0f : ((dx > 0) ? 1.0f : -1.0f);
	if (dx != 0.0f) {
		dx += dxMax * brX * (-dxS) * dt;
		if ((dx / abs(dx)) != dxS)
			dx = 0.0f;
	}
}

void Entity::applyGravity(double dt) {
	dy += accY * 2.0f * dt;
	if (dy >= dyMax)
		dy = dyMax;
}

void Entity::handleCollisions(double dt) {
	updateHitBoxes(dt);

	bool vCollide = false;
	bool hCollide = false;

	int vX0 = (int)(vBox.left / C::GRID_SIZE);
	int vX1 = (int)((vBox.left + vBox.width - C::EPS) / C::GRID_SIZE);
	int vY0 = (int)((vBox.top + C::EPS) / C::GRID_SIZE);
	int vY1 = (int)((vBox.top + vBox.height) / C::GRID_SIZE);

	int hX0 = (int)(hBox.left / C::GRID_SIZE);
	int hX1 = (int)((hBox.left + hBox.width) / C::GRID_SIZE);
	int hY0 = (int)((hBox.top + C::EPS) / C::GRID_SIZE);
	int hY1 = (int)((hBox.top + hBox.height - C::EPS) / C::GRID_SIZE);


	for (int i = vX0; i <= vX1; i++)
		if ((speedY > 0.0f && wallMap.isWall(i, vY1)) || (speedY < 0.0f && wallMap.isWall(i, vY0))) {
			vCollide = true;
			break;
		}
	for (int i = hY0; i <= hY1; i++)
	for (int i = hY0; i <= hY1; i++)
		if ((speedX > 0.0f && wallMap.isWall(hX1, i)) || (speedX < 0.0f && wallMap.isWall(hX0, i))) {
			hCollide = true;
			break;
		}

	isGrounded = false;
	double dy0 = speedY;

	if (vCollide) {
		if (speedY > 0.0f) isGrounded = true; else isGrounded = false;
		dy = 0.0f;
		speedY = 0.0f;
	}

	if (hCollide) {
		dx = 0.0f;
		speedX = 0.0f;
	}

	updateHitBoxes(dt);
	checkGround(dy0);
	justJump = false;
}

void Entity::updateHitBoxes(double dt) {
	sf::Vector2f orgn = animSprite.sprite.getOrigin();
	sf::Vector2f scaledOrgn = { orgn.x * scale.x, orgn.y * scale.y };
	vBox.left = pos.x - scaledOrgn.x + leftPaddingBox;
	vBox.top = (pos.y - scaledOrgn.y + topPaddingBox) + (speedY * dt);
	hBox.left = (pos.x - scaledOrgn.x + leftPaddingBox) + (speedX * dt);
	hBox.top = pos.y- scaledOrgn.y + topPaddingBox;
}

void Entity::checkGround(double dy0) {
	if (dy0 < 0.0f || justJump) return;
	int x0 = (int)(hBox.left / C::GRID_SIZE);
	int x1 = (int)((hBox.left + hBox.width - C::EPS) / C::GRID_SIZE);
	int gY = (int)((hBox.top + hBox.height + 1.0f) / C::GRID_SIZE);
	isGrounded = false;
	for (int i = x0; i <= x1; i++)
		if (wallMap.isWall(i, gY)) { isGrounded = true; break; }
}

void Entity::jump() {
	if (!isGrounded) return;
	dy = -1000.0f;
	isGrounded = false;
	justJump = true;
}

int Entity::getSense() {
	return (int)std::copysign(1.0f, animSprite.sprite.getScale().x);
}

void Entity::setSense(float sign) {
	sf::Vector2f scl = animSprite.sprite.getScale();
	if (std::signbit(sign) == std::signbit(scl.x)) return;
	sf::Vector2f result = { std::copysign(scl.x, sign), scl.y};
	animSprite.sprite.setScale(result);
}

void Entity::handleAnimations(double dt) {
	if (isDead)
		animSprite.playAnim(Death);
	else if (dx == 0.0f && dy == 0.0f)
		animSprite.playAnim(Idle);
	else if (dy > 250.0f)
		animSprite.playAnim(Fall);
	else if (dy < -0.0f)
		animSprite.playAnim(Jump);
	else if ((dx > 0.0f && isGrounded && getSense() > 0) || (dx < 0.0f && isGrounded && getSense() < 0))
		animSprite.playAnim(Run);
	else if ((dx < 0.0f && isGrounded && getSense() > 0) || (dx > 0.0f && isGrounded && getSense() < 0))
		animSprite.playAnim(RunBack);

	animSprite.update(dt);
}

void Entity::handleDmgFeedback(double dt) {
	if (!isTakingDamage) return;
	dmgTimer += dt;
	if (dmgTimer < dmgEffTime) return;
	isTakingDamage = false;
	animSprite.sprite.setColor(sf::Color(255, 255, 255));
}

void Entity::takeDamage(sf::Vector2f dir) {
	life--;
	applyRecoil(3000.0f, dir);
	isTakingDamage = true;
	dmgTimer = 0.0;
	animSprite.sprite.setColor(sf::Color(255, 0, 0));
	if (isPlayer) wallMap.shakeCamera(0.3f, 4.0f);
	if (life <= 0) die();
}

void Entity::applyRecoil(float force, float aimedAngle) {
	sf::Vector2f recoil = -getForward(aimedAngle);
	addedX += recoil.x * force;
	if (!isGrounded) addedY += recoil.y * force;
}

void Entity::applyRecoil(float force, sf::Vector2f dir) {
	sf::Vector2f recoil = normalize(dir);
	addedX += recoil.x * force;
	if (!isGrounded) addedY += recoil.y * force;
}

void Entity::die() {
	isDead = true;
	animSprite.playAnim(Death);
}

std::vector<sf::Vector2i> Entity::getVHitboxCells() {
	int vX0 = (int)(vBox.left / C::GRID_SIZE);
	int vX1 = (int)((vBox.left + vBox.width - C::EPS) / C::GRID_SIZE);
	int vY0 = (int)((vBox.top + C::EPS) / C::GRID_SIZE);
	int vY1 = (int)((vBox.top + vBox.height) / C::GRID_SIZE);

	std::vector<sf::Vector2i> cells;
	if (vX1 < vX0 || vY1 < vY0) return cells;

	cells.reserve((vX1 - vX0 + 1) * (vY1 - vY0 + 1));

	for (int cy = vY0; cy <= vY1; ++cy)
		for (int cx = vX0; cx <= vX1; ++cx)
			cells.emplace_back(cx, cy);

	return cells;
}

//debug
void Entity::setBoxShapes() {
	vBoxShape.setSize({ vBox.width, vBox.height });
	vBoxShape.setFillColor(sf::Color::Transparent);
	vBoxShape.setOutlineThickness(2.0f);
	vBoxShape.setOutlineColor(sf::Color::Red);

	hBoxShape.setSize({ hBox.width, hBox.height });
	hBoxShape.setFillColor(sf::Color::Transparent);
	hBoxShape.setOutlineThickness(2.0f);
	hBoxShape.setOutlineColor(sf::Color::Green);
}

void Entity::drawHitBoxes(sf::RenderWindow& win) {
	vBoxShape.setPosition({ vBox.left, vBox.top });
	hBoxShape.setPosition({ hBox.left, hBox.top });
	win.draw(vBoxShape);
	win.draw(hBoxShape);
}

