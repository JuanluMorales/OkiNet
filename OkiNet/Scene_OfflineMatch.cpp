#include "Scene_OfflineMatch.h"

void Scene_OfflineMatch::Init(GameState* stateMan)
{
	stateManager = stateMan;

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
	playerTwoStartPos = sf::Vector2f(200, 52);
	playerTwo.InitCharacter(PlayerCharacter::PlayerID::PlayerTwo, playerTwoStartPos);

}

void Scene_OfflineMatch::OverrideRender()
{
	// render something to test
	//window->draw(platform);

	window->draw(playerOne.bodyColl);
	window->draw(playerTwo.bodyColl);

	window->draw(playerOne);
	window->draw(playerTwo);



	// Render font
	window->draw(DebugText);


}

void Scene_OfflineMatch::OverrideUpdate(float dt)
{
	playerOne.update(dt, window);
	playerTwo.update(dt, window);

	// Check collision
	// For player one

	Collision::CollisionResponse newColl = Collision::checkBoundingBox_Sides(&playerOne.bodyColl, &playerTwo.bodyColl);
	if (newColl.None == false)
	{
		
		DebugText.setString("COLLISION");
	}else DebugText.setString("NO COLLISION");

}

void Scene_OfflineMatch::OverrideHandleInput(float dt)
{
	if (input->isKeyDown(sf::Keyboard::Escape)) {
		input->SetKeyUp(sf::Keyboard::Escape);
		window->close();
	}

	playerOne.handleInput(input, dt);
	playerTwo.handleInput(input, dt);


}
