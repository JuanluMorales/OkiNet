#pragma once
#include "GameState.h"
#include "PlayerCharacter.h"
#include "Collision.h"

// Scene that comprises the offline version of the game
class Scene_OfflineMatch : public Scene
{
public:
	// Make sure to allow use of the default constructor
	using Scene::Scene;

	void Init(GameState* stateManager);
	void OverrideEarlyUpdate(float dt) override;
	void OverrideRender() override;
	void OverrideUpdate(float dt) override;
	void OverrideHandleInput(float dt) override;

private:
	GameState* stateManager;

	PlayerCharacter playerOne;
	PlayerCharacter playerTwo;

	sf::Vector2f playerOneStartPos;
	sf::Vector2f playerTwoStartPos;

	// Map limits
	CollisionBox leftColl;
	CollisionBox rightColl;

	sf::Font DebugFont;
	sf::Text DebugText;

	
};

