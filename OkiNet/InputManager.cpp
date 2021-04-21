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

bool InputManager::IsKeyDown(int key) {
	return keys[key];
}

void InputManager::SetMouseX(int lx) {
	mouse.x = lx;
}

void InputManager::SetMouseY(int ly) {
	mouse.y = ly;
}

void InputManager::SetMousePosition(int lx, int ly) {
	SetMouseX(lx);
	SetMouseY(ly);
}

int InputManager::GetMouseX() {
	return mouse.x;
}

int InputManager::GetMouseY() {
	return mouse.y;
}

void InputManager::SetMouseLeftDown(bool down) {
	mouse.left = down;
}

bool InputManager::IsMouseLeftDown() {
	return mouse.left;
}

void InputManager::SetMouseRightDown(bool down) {
	mouse.right = down;
}
bool InputManager::IsMouseRightDown() {
	return mouse.right;
}

void InputManager::WriteInputField(char newText)
{
	inputText += newText;
}

void InputManager::SetInputField(char newText)
{
	inputText = newText;
}
void InputManager::SetInputField(std::string newText)
{
	inputText = newText;
}
void InputManager::ClearInputField()
{
	inputText.clear();
}

void InputManager::RemoveLetterFromInputField()
{
	inputText.pop_back();
}

