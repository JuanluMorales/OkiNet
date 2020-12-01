#include "Scene_OfflineMatch.h"

void Scene_OfflineMatch::Init()
{
	platform.setSize(sf::Vector2f(1500, 100));
	platform.setPosition(-700, 300);
	platform.setFillColor(sf::Color::Green);

	// Setup debug font and text
	DebugFont.loadFromFile("font/arial.ttf");
	DebugText.setFont(DebugFont);
	DebugText.setString("DEBUG: SCENE -> OFFLINE_SCENE");
	DebugText.setCharacterSize(15);
	DebugText.setOrigin(sf::Vector2f(DebugText.getGlobalBounds().width / 2, DebugText.getGlobalBounds().height / 2));
	DebugText.setPosition(sf::Vector2f(-400, 330));
	DebugText.setFillColor(sf::Color::Red);


	// Setup players
	playerOneStartPos = sf::Vector2f(-70, 25);
	playerTwoStartPos = sf::Vector2f(100, 25);
	playerOne.InitCharacter(PlayerCharacter::PlayerID::PlayerOne, playerOneStartPos);
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
}
