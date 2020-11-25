#include "InputManager.h"

//Class that contains all the input related methods to capture what key is being pressed or released
//Also handles mouse position and clicks

void InputManager::SetKeyDown(int key) {
	if (key >= 0) {
		keys[key] = true;
	}
}

void InputManager::SetKeyUp(int key) {
	if (key >= 0) {
		keys[key] = false;
	}
}

bool InputManager::isKeyDown(int key) {
	return keys[key];
}

void InputManager::setMouseX(int lx) {
	mouse.x = lx;
}

void InputManager::setMouseY(int ly) {
	mouse.y = ly;
}

void InputManager::setMousePosition(int lx, int ly) {
	setMouseX(lx);
	setMouseY(ly);
}

int InputManager::getMouseX() {
	return mouse.x;
}

int InputManager::getMouseY() {
	return mouse.y;
}

void InputManager::setMouseLeftDown(bool down) {
	mouse.left = down;
}

bool InputManager::isMouseLeftDown() {
	return mouse.left;
}

void InputManager::setMouseRightDown(bool down) {
	mouse.right = down;
}
bool InputManager::isMouseRightDown() {
	return mouse.right;
}

