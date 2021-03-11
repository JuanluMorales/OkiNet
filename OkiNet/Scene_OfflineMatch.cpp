#include "Scene_OfflineMatch.h"

void Scene_OfflineMatch::Init(GameState* stateMan)
{
	stateManager = stateMan;

	// Setup debug font and text
	DebugFont.loadFromFile("font/arial.ttf");
	DebugText.setFont(DebugFont);
	DebugText.setString("[DEBUG_OUTPUT] SCENE: OFFLINE_SCENE");
	DebugText.setCharacterSize(15);
	DebugText.setOrigin(sf::Vector2f(DebugText.getGlobalBounds().width / 2, DebugText.getGlobalBounds().height / 2));
	DebugText.setPosition(sf::Vector2f(0, 350));
	DebugText.setFillColor(sf::Color::Red);


	// Setup player one
	playerOneStartPos = sf::Vector2f(-200, 0);
	playerOne.InitCharacter(PlayerCharacter::PlayerID::PlayerOne, playerOneStartPos);

	// Setup player two
	playerTwoStartPos = sf::Vector2f(200, 0);
	playerTwo.InitCharacter(PlayerCharacter::PlayerID::PlayerTwo, playerTwoStartPos);

}

void Scene_OfflineMatch::OverrideEarlyUpdate(float dt)
{
}

void Scene_OfflineMatch::OverrideRender()
{
	// draw players
	if (playerOne.IsActive()) window->draw(playerOne);
	if (playerTwo.IsActive()) window->draw(playerTwo);

	// draw player collision
	for (auto coll : playerOne.GetCurrentCollision())
	{
		if (coll->GetDrawable() && coll->IsActive()) window->draw(*coll);
	}
	for (auto coll : playerTwo.GetCurrentCollision())
	{
		if (coll->GetDrawable() && coll->IsActive()) window->draw(*coll);
	}

	// Render font
	window->draw(DebugText);


}

void Scene_OfflineMatch::OverrideUpdate(float dt)
{
	playerOne.Update(dt, window);
	playerTwo.Update(dt, window);

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

void Scene_OfflineMatch::OverrideHandleInput(float dt)
{
	if (input->isKeyDown(sf::Keyboard::Escape)) {
		input->SetKeyUp(sf::Keyboard::Escape);
		window->close();
	}

	playerOne.HandleInput(input, dt);
	playerTwo.HandleInput(input, dt);


}
