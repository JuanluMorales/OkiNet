#pragma once
#include <net_common_headers.h> // Network static libraries
#include "GameState.h"
#include "PlayerCharacter.h"

class Scene_OnlineMatch : public Scene
{
public:
	// Make sure to allow use of the default constructor
	using Scene::Scene;

	// False = Host
	void Init(GameState* stateMan);
	void OverrideRender();
	void OverrideUpdate(float dt);
	void OverrideHandleInput(float dt);

	bool AttemptConnection();
	void GrabSomeData(asio::ip::tcp::socket& socket);

	bool PlayerTwoConnected;

private:
	GameState* stateManager;

	SpriteBase platform;
	PlayerCharacter playerOne;
	PlayerCharacter playerTwo;

	sf::Vector2f playerOneStartPos;
	sf::Vector2f playerTwoStartPos;

	sf::Font DebugFont;
	sf::Text DebugText;



	// network variables
	std::vector<char> vBuffer; // Large buffer to store messages
	
};

