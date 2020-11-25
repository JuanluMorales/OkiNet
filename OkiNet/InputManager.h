#pragma once
// Class that handles receiving input from mouse and keyboard
class InputManager
{
private:
	//Structure to represent the Mouse
	struct Mouse
	{
		int x, y = 0;
		bool left = false; //Initialize on false as predetermined
		bool right = false;

	};
public:
	void SetKeyDown(int key);
	void SetKeyUp(int key);
	bool isKeyDown(int key);

	void setMouseX(int lx);
	void setMouseY(int ly);
	void setMousePosition(int lx, int ly);
	int getMouseX();
	int getMouseY();
	void setMouseLeftDown(bool down);
	bool isMouseLeftDown();
	void setMouseRightDown(bool down);
	bool isMouseRightDown();

private:
	bool keys[256]{ false };
	Mouse mouse;
};