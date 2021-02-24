#pragma once
#include <SFML/Graphics.hpp>
#include "InputManager.h"
#include "SpriteBase.h"
#include <iostream> // For debugging purposes

// Class that holds the base instance of a scene to be overriden by child classes to implement functionality
class Scene
{
public:
	Scene(sf::RenderWindow* hwnd, InputManager* inpt, sf::View* vw);
	~Scene();

	// Call RunScene() to execute input, update and render
	void RunScene(float dt);

	// Override if necessary to add initialization of variables and object or other functionality
	virtual void Init();
	// Methods called as overrides on child class to add functionality to each 
	virtual void OverrideRender();
	virtual void OverrideUpdate(float dt);
	virtual void OverrideHandleInput(float dt);

	void CreateRectShape(sf::RectangleShape rectShape, float sizeX, float sizeY, float posX, float posY, sf::Color color);

private:
	void HandleInput(float dt);
	void Update(float dt);
	void Render();

	//Clear window to redraw
	void BeginDraw();
	//Display the screen with what has been drawn
	void EndDraw();

protected:
	sf::RenderWindow* window;
	sf::View* view;
	InputManager* input;
	sf::Color clearColor;
};

