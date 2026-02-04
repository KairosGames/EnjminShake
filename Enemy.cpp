#include "Enemy.h"
#include "WallMap.h"

Enemy::Enemy(sf::Vector2f pPos, WallMap& pWallMap, Player& pPlayer, const std::string& spritePath, sf::Vector2i frameSize)
	: Entity(pWallMap, spritePath, frameSize),
	player(pPlayer),
	weapon("res/sprites/m4.png", { 74, 32 })
{
	isPlayer = false;
	isGoingRight = randBool();
	dxMax = 300.0f;
	dxRMax = 200.0f;
	patrolSpeed = 150.0f;
	brX = 2.0f;
	scale = { 2.0f, 2.0f };
	animSprite.sprite.setScale(scale);
	renderSize = { animSprite.frameSize.x * scale.x , animSprite.frameSize.y * scale.y };
	leftPaddingBox = 13 * scale.x;
	rightPaddingBox = 13 * scale.x;
	topPaddingBox = 7 * scale.y;
	bottPaddingBox = 6 * scale.y;
	startPos = pPos;
	pos = pPos;
	setPos(pPos.x, pPos.y);
	setHitBoxes();
	loadAnimations();
	lookDownOffset = { (vBox.width / 2.0f) + 20.0f, (vBox.height / 2.0f) + 10.0f };
	lookDownPos = pos + lookDownOffset;

	bulletTex.loadFromFile("res/sprites/bullet.png");
	weapOffset = { 0.0f, 12.0f };
	weapScale = { 1.2f, 1.2f };
	weapOrigin = { 27.0f, 15.0f };
	lastAimSense = (isGoingRight) ? 1 : -1;
	weapon.sprite.setScale(weapScale);
	weapon.sprite.setOrigin(weapOrigin);
	weapPos = pPos;
	weapon.sprite.setPosition(pPos);

	if (C::IS_DEBUG) setBoxShapes();
}

void Enemy::update(double dt) {
	updateState();
	doAction(dt);
	updateSense();
	if (!isDead) updateWeapon(dt);
	Entity::update(dt);
}

void Enemy::draw(sf::RenderWindow& win) {
	Entity::draw(win);
	if (!isDead) weapon.draw(&win);
}

void Enemy::setForEditorInstance() {
	animSprite.playAnim(Entity::AnimType::Idle);
	targetPos = pos + weapOffset;
	weapPos = targetPos;
	weapon.sprite.setPosition(targetPos);
}

void Enemy::loadAnimations() {
	animSprite.addAnim(Idle, std::vector<unsigned int>{ 11, 12, 13, 14, 15 }, 0.5, true);
	animSprite.addAnim(Run, std::vector<unsigned int>{ 17, 18, 19, 20, 21, 22, 23, 24 }, 0.45, true);
	animSprite.addAnim(RunBack, std::vector<unsigned int>{ 24, 23, 22, 21, 20, 19, 18, 17 }, 0.8, true);
	animSprite.addAnim(Jump, std::vector<unsigned int>{ 16 }, 0.25, true);
	animSprite.addAnim(Fall, std::vector<unsigned int>{ 8, 9, 10 }, 0.15, true);
	animSprite.addAnim(Death, std::vector<unsigned int>{ 0, 1, 2, 3, 4, 5, 6, 7 }, 1.0, false);

	weapon.addAnim(Wait, std::vector<unsigned int>{ 0, 1, 2, 3, 4 }, 0.5, true);
	weapon.addAnim(Shoot, std::vector<unsigned int>{ 5, 6, 7, 8, 9, 10, 11, 12 }, 0.05, false);
}

void Enemy::updateState() {
	float distSquaredToPlayer = getDistanceSquared(pos, player.pos);
	float distXToStart = abs(pos.x - startPos.x);
	bool lookAtPlayer = ((player.pos.x - pos.x >= 0) ? 1 : -1) == getSense();

	if (isDead) {
		currentState = Dead;
	}
	else if (distSquaredToPlayer <= viewDistance * viewDistance && lookAtPlayer && !player.isDead && canSeePlayer()) {
		hasSeenPlayer = true;
		currentState = Attack;
	}
	else if (hasSeenPlayer && distXToStart <= patrolDistance && !player.isDead) {
		currentState = Chase;
	}
	else if (distXToStart > patrolDistance) {
		hasSeenPlayer = false;
		currentState = Patrol;
	}
	else {
		currentState = Patrol;
	}
}

void Enemy::doAction(double dt) {
	if (currentState == Patrol) {
		patrol(dt);
	}
	else if (currentState == Chase) {
		if (!isGrounded) return;
		if (!canSeePlayer()){
			hasSeenPlayer = false;
			return;
		}
		if (player.pos.x > pos.x)
			moveX(dt, true, dxMax);
		else
			moveX(dt, false, dxMax);
	}
	else if (currentState == Attack) {
		stopMoveX(dt);
		shoot(dt);
	}
	else if (currentState == Dead) {
		stopMoveX(dt);
	}
}

void Enemy::updateSense() {
	if (currentState == Patrol || currentState == Chase)
		setSense(dx);
	else if (currentState == Attack)
		setSense(player.pos.x - pos.x);
}

void Enemy::updateWeapon(double dt) {
	updateWeaponPosition(dt);
	updateWeaponSense(dt);
	updateWeaponAnimations(dt);
	weapon.update(dt);
	updateShootTimer(dt);
}

void Enemy::updateWeaponPosition(double dt) {
	float offsetX = weapOffset.x * getSense();
	targetPos = { pos.x + offsetX, pos.y + weapOffset.y };
	weapPos = lerpVec(weapPos, targetPos, 0.1f, dt);
	weapon.sprite.setPosition(weapPos);
}

void Enemy::updateWeaponSense(double dt) {
	float sclX = weapScale.x * getSense();
	weapon.sprite.setScale(sclX, weapScale.y);
	aim(dt);
}

void Enemy::aim(double dt) {
	int sense = (getSense() > 0.f) ? 1 : -1;
	int prevSense = (lastAimSense > 0) ? 1 : -1;

	if (!aimRotInit) {
		aimRotCont = weapon.sprite.getRotation();
		lastAimSense = sense;
		aimRotInit = true;
		prevSense = sense;
	}

	auto normDelta = [](float a) {
		a = std::fmod(a + 180.0f, 360.0f);
		if (a < 0.0f) a += 360.0f;
		return a - 180.0f;
	};

	if (sense != prevSense) {
		aimRotCont -= 180.0f;
		lastAimSense = sense;
	}

	float targetAngle = 0.0f;
	if (currentState == Attack) {
		sf::Vector2f target = player.weapon.targetPos;
		sf::Vector2f src = weapon.sprite.getPosition();
		sf::Vector2f d{ target.x - src.x, target.y - src.y };
		if (d.x == 0.0f && d.y == 0.0f) return;
		aimedAngle = std::atan2(d.y, d.x) * 180.0f / C::PI;
		targetAngle = (sense > 0) ? aimedAngle : (aimedAngle - 180.0f);
	}

	float diff = normDelta(targetAngle - aimRotCont);
	double t = 12.0 * dt;
	if (t > 1.0) t = 1.0;
	aimRotCont += diff * t;
	float out = std::fmod(aimRotCont, 360.0f);
	if (out < 0.0f) out += 360.0f;
	weapon.sprite.setRotation(out);
}

void Enemy::updateWeaponAnimations(double dt) {
	if (weapon.current == nullptr)
		weapon.playAnim(Wait);
	else if (weapon.current == &weapon.animations[Shoot] && weapon.current->isFinished)
		weapon.playAnim(Wait);
}

void Enemy::updateShootTimer(double dt) {
	if (currentState == Attack && !canShoot) {
		reloadTimer += dt;
		if (reloadTimer >= reloadTime) {
			canShoot = true;
			reloadTimer = 0.0f;
		}
		return;
	}
	canShoot = false;
	reloadTimer = 0.0f;
}

void Enemy::shoot(double dt) {
	if (!canShoot) return;
	sf::Vector2f firePos = getFirePos();
	weapon.playAnim(Shoot);
	playShootEffect(firePos);
	generateBullet(firePos);
	applyRecoil(1500.0f, aimedAngle);
	canShoot = false;
	reloadTimer = 0.0f;
}

sf::Vector2f Enemy::getFirePos() {
	float frwdOffset = 65.0f;
	float upOffset = -2.0f * getSense();
	sf::Vector2f firePos = weapon.sprite.getPosition() + (getForward(aimedAngle) * frwdOffset) + (getUp(aimedAngle) * upOffset);
	return firePos;
}

void Enemy::playShootEffect(sf::Vector2f firePos) {
	EffectsManager::Instance().playAnimEffect(EffectsManager::AnimEffectType::FireMuzzle, firePos, aimedAngle, { 4.0f, 4.0f });
}

void Enemy::generateBullet(sf::Vector2f firePos) {
	Bullet newBullet(bulletTex, firePos, aimedAngle);
	wallMap.bullets.push_back(newBullet);
}

void Enemy::patrol(double dt) {
	if (!isGrounded) return;

	float dir = (isGoingRight) ? 1.0f : -1.0f;
	lookDownPos = { pos.x + (lookDownOffset.x * dir), pos.y + lookDownOffset.y };

	if (!isThereGround() || !canGoForward()) {
		isGoingRight = !isGoingRight;
		return;
	}

	moveX(dt, isGoingRight, patrolSpeed);
	if (abs(pos.x - startPos.x) > patrolDistance) isGoingRight = !isGoingRight;
	if ((pos.x >= startPos.x + patrolDistance && isGoingRight) || (pos.x <= startPos.x - patrolDistance && !isGoingRight))
		isGoingRight = !isGoingRight;
}

bool Enemy::isThereGround() {
	int px = (int)(lookDownPos.x / C::GRID_SIZE);
	int py = (int)(lookDownPos.y / C::GRID_SIZE);
	return wallMap.isWall(px, py);
}

bool Enemy::canGoForward() {
	int px = (int)(lookDownPos.x / C::GRID_SIZE);
	int py = (int)((pos.y) / C::GRID_SIZE);
	return !wallMap.isWall(px, py);
}

bool Enemy::canSeePlayer() {
	int startX = int(pos.x / C::GRID_SIZE);
	int startY = int(pos.y / C::GRID_SIZE);
	int endX = int(player.pos.x / C::GRID_SIZE);
	int endY = int(player.pos.y / C::GRID_SIZE);
	int x0 = startX;
	int y0 = startY;
	int x1 = endX;;
	int y1 = endY;

	bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);
	if (steep) {
		std::swap(x0, y0);
		std::swap(x1, y1);
	}

	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	const int dx = x1 - x0;
	const int dy = std::abs(y1 - y0);
	int err = dx / 2;
	int y = y0;
	const int yStep = (y0 < y1) ? 1 : -1;

	for (int x = x0; x <= x1; ++x)
	{
		const int cellX = steep ? y : x;
		const int cellY = steep ? x : y;

		if ((cellX != startX || cellY != startY) && wallMap.isWall(cellX, cellY))
			return false;

		err -= dy;
		if (err < 0) {
			y += yStep;
			err += dx;
		}
	}
	return true;
}
