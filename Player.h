#pragma once

#include <algorithm>
#include <iostream>
#include <string>

#include <SFML/System/Vector2.hpp>
#include "Entity.h"
#include "Pointer.h"
#include "PlayerWeapon.h"

class Player : public Entity
{
public:
    Pointer& pointer;
    PlayerWeapon weapon;
    bool wasSpacePressed;
    bool isGameInEditor;

    Player(WallMap& pWallMap, const std::string& spritePath, sf::Vector2i frameSize, Pointer& pPointer);
    void update(double dt) override;
    void draw(sf::RenderWindow& win) override;
    void getInputs(double dt);
    void onSpacePressed();
    void loadAnimations();
    void updateSense();
    void setForEditorInstance(sf::Vector2f pPos);
};