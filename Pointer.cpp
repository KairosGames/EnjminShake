#include "Pointer.h"


Pointer::Pointer(sf::RenderWindow& pWin) : win(pWin) {

}

void Pointer::update(double dt) {
	winPos = sf::Mouse::getPosition(win);
	worldPos = win.mapPixelToCoords(winPos);
}

void Pointer::draw() {

}