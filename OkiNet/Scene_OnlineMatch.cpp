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
	DebugText.setPosition(sf::Vector2f(0, -200));
	DebugText.setFillColor(sf::Color::Red);


	// Setup player one
	playerOneStartPos = sf::Vector2f(-200, 0);
	playerOne.InitCharacter(PlayerCharacter::PlayerID::PlayerOne, playerOneStartPos);

	// Setup player two
	PlayerTwoConnected = false;
	playerTwoStartPos = sf::Vector2f(200, 0);
	playerTwo.InitCharacter(PlayerCharacter::PlayerID::PlayerTwo, playerTwoStartPos);

}

void Scene_OnlineMatch::InitAsHost(GameState* stateMan, std::string& port)
{
	Init(stateMan); // Initialize normally
	isHost = true;
	int strPort = std::stoi(port); // convert string to int
	client = new CustomHostClient(strPort);
	client->StartListening();


}

void Scene_OnlineMatch::InitAsClient(GameState* stateMan, std::string& ip, std::string& port)
{
	Init(stateMan); // Initialize normally
	isHost = false;

	bool connSuccesful = false;
	// Attempt connection

	// if failed go back to main menu
	if (!connSuccesful) stateMan->GoToScene(scenes::MainMenu);

}

void Scene_OnlineMatch::OverrideRender()
{
	// draw players
	if (playerOne.IsActive()) window->draw(playerOne);
	if (PlayerTwoConnected) if (playerTwo.IsActive()) window->draw(playerTwo);

	// draw player collision
	for (auto coll : playerOne.GetCurrentCollision())
	{
		if (coll->GetDrawable() && coll->IsActive()) window->draw(*coll);
	}
	for (auto coll : playerTwo.GetCurrentCollision())
	{
		if (PlayerTwoConnected) if (coll->GetDrawable() && coll->IsActive()) window->draw(*coll);
	}

	// Render font
	window->draw(DebugText);


}

void Scene_OnlineMatch::OverrideUpdate(float dt)
{

	client->Update();

	playerOne.Update(dt, window);
	if (PlayerTwoConnected) playerTwo.Update(dt, window);

	// Iterate all current's frame collision boxes for both players
	for (auto collA : playerOne.GetCurrentCollision())
	{
		for (auto collB : playerTwo.GetCurrentCollision())
		{
			if (collA->IsActive() && collB->IsActive())
			{
				Collision::CollisionResponse newColl = Collision::checkBoundingBox_Sides(collA, collB);

				if (newColl.None) // If there was no collision...
				{
					playerOne.NoCollisionRegistered();
					playerTwo.NoCollisionRegistered();

					DebugText.setString("NO COLLISION");
				}
				else
				{
					playerOne.CollisionResponseToPlayer(&newColl);
					playerTwo.CollisionResponseToPlayer(&newColl);

					DebugText.setString("COLLISION");
				}
			}
		}
	}

	
}

void Scene_OnlineMatch::OverrideHandleInput(float dt)
{
	if (input->IsKeyDown(sf::Keyboard::Escape)) {
		input->SetKeyUp(sf::Keyboard::Escape);
		window->close();
	}

	playerOne.HandleInput(input, dt);

}
