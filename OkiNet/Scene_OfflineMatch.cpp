#include "Scene_OfflineMatch.h"

void Scene_OfflineMatch::Init()
{
	platform.setSize(sf::Vector2f(1500, 100));
	platform.setPosition(-700, 300);
	platform.setFillColor(sf::Color::Green);

	// Setup debug font and text
	DebugFont.loadFromFile("font/arial.ttf");
	DebugText.setFont(DebugFont);
	DebugText.setString("[DEBUG_OUTPUT] SCENE: OFFLINE_SCENE");
	DebugText.setCharacterSize(15);
	DebugText.setOrigin(sf::Vector2f(DebugText.getGlobalBounds().width / 2, DebugText.getGlobalBounds().height / 2));
	DebugText.setPosition(sf::Vector2f(0, 350));
	DebugText.setFillColor(sf::Color::Red);


	// Setup player one
	playerOneStartPos = sf::Vector2f(-500, 52);
	playerOne.InitCharacter(PlayerCharacter::PlayerID::PlayerOne, playerOneStartPos);

	// Setup player two
	playerTwoStartPos = sf::Vector2f(350, 8);	
	playerTwo.InitCharacter(PlayerCharacter::PlayerID::PlayerTwo, playerTwoStartPos);

}

void Scene_OfflineMatch::OverrideRender()
{
	// render something to test
	window->draw(platform);

	window->draw(playerOne);
	window->draw(playerTwo);

	// Render font
	window->draw(DebugText);


}

void Scene_OfflineMatch::OverrideUpdate(float dt)
{
}

void Scene_OfflineMatch::OverrideHandleInput(float dt)
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

	if (input->isKeyDown(sf::Keyboard::Left))
	{
		playerTwo.setPosition(playerTwo.getPosition().x - playerTwo.MoveSpeed * dt, playerTwo.getPosition().y);
	}
	if (input->isKeyDown(sf::Keyboard::Right))
	{
		playerTwo.setPosition(playerTwo.getPosition().x + playerTwo.MoveSpeed * dt, playerTwo.getPosition().y);
	}
}
