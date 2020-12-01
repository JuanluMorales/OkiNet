#include "Scene_OnlineMatch.h"

void Scene_OnlineMatch::Init(GameState* stateMan)
{
	stateManager = stateMan;

	platform.setSize(sf::Vector2f(1500, 100));
	platform.setPosition(-700, 300);
	platform.setFillColor(sf::Color::Green);

	// Setup debug font and text
	DebugFont.loadFromFile("font/arial.ttf");
	DebugText.setFont(DebugFont);
	DebugText.setString("[DEBUG_OUTPUT] SCENE: ONLINE_SCENE");
	DebugText.setCharacterSize(15);
	DebugText.setOrigin(sf::Vector2f(DebugText.getGlobalBounds().width / 2, DebugText.getGlobalBounds().height / 2));
	DebugText.setPosition(sf::Vector2f(0, 350));
	DebugText.setFillColor(sf::Color::Red);


	// Setup player one
	playerOneStartPos = sf::Vector2f(-500, 52);
	playerOne.InitCharacter(PlayerCharacter::PlayerID::PlayerOne, playerOneStartPos);

	// Setup player two in await mode
	PlayerTwoConnected = false;
	playerTwoStartPos = sf::Vector2f(350, 8);
	playerTwo.InitCharacter(PlayerCharacter::PlayerID::PlayerTwo, playerTwoStartPos);

}

void Scene_OnlineMatch::OverrideRender()
{
	// render something to test
	window->draw(platform);

	window->draw(playerOne);

	if (PlayerTwoConnected) 
	{
		window->draw(playerTwo);
	}

	// Render font
	window->draw(DebugText);


}

void Scene_OnlineMatch::OverrideUpdate(float dt)
{

	// Update debug output on connection status
	if(PlayerTwoConnected) DebugText.setString(DebugText.getString() + " Succesful connection from Player 2");
}

void Scene_OnlineMatch::OverrideHandleInput(float dt)
{
	if (input->isKeyDown(sf::Keyboard::Escape)) {
		input->SetKeyUp(sf::Keyboard::Escape);
		window->close();
	}

	// Placeholder movement
	if (input->isKeyDown(sf::Keyboard::A))
	{
		playerOne.setPosition(playerOne.getPosition().x - playerOne.MoveSpeed * dt, playerOne.getPosition().y);
	}
	if (input->isKeyDown(sf::Keyboard::D))
	{
		playerOne.setPosition(playerOne.getPosition().x + playerOne.MoveSpeed * dt, playerOne.getPosition().y);
	}
}
