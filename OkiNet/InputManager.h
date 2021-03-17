#pragma once
#include <string>
// Class that handles receiving input from mouse and keyboard
class InputManager
{
private:
	//Structure to represent the Mouse
	struct Mouse
	{
		int x = 0;
		int y = 0;
		bool left = false; //Initialize on false as predetermined
		bool right = false;

	};
public:
	void SetKeyDown(int key);
	void SetKeyUp(int key);
	bool IsKeyDown(int key);

	void SetMouseX(int lx);
	void SetMouseY(int ly);
	void SetMousePosition(int lx, int ly);
	int GetMouseX();
	int GetMouseY();
	void SetMouseLeftDown(bool down);
	bool IsMouseLeftDown();
	void SetMouseRightDown(bool down);
	bool IsMouseRightDown();

	void SetTextInputFieldActive(bool b) { isAcceptingInputText = b; };
	bool IsTextInputFieldActive() { return isAcceptingInputText; };
	void WriteInputField(char newText);
	void ClearInputField();
	std::string GetCurrentInputText() { return inputText; };
	void RemoveLetterFromInputField();

private:
	bool keys[256]{ false };
	Mouse mouse;

	bool isAcceptingInputText = false;
	std::string inputText;
};