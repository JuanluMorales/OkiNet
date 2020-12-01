#pragma once
#include "GameState.h"
#include "PlayerCharacter.h"

class Scene_OnlineMatch : public Scene
{
public:
	// Make sure to allow use of the default constructor
	using Scene::Scene;

	void Init(GameState* stateMan);
	void OverrideRender();
	void OverrideUpdate(float dt);
	void OverrideHandleInput(float dt);

private:
	GameState* stateManager;

	SpriteBase platform;
	PlayerCharacter playerOne;
	PlayerCharacter playerTwo;

	sf::Vector2f playerOneStartPos;
	sf::Vector2f playerTwoStartPos;

	sf::Font DebugFont;
	sf::Text DebugText;

	bool PlayerTwoConnected;
};

