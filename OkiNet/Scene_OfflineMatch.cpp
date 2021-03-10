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
	playerOneStartPos = sf::Vector2f(-600, -200);
	playerOne.InitCharacter(PlayerCharacter::PlayerID::PlayerOne, playerOneStartPos);

	// Setup player two
	playerTwoStartPos = sf::Vector2f(-150, -200);
	playerTwo.InitCharacter(PlayerCharacter::PlayerID::PlayerTwo, playerTwoStartPos);

}

void Scene_OfflineMatch::OverrideRender()
{
	// render something to test
	//window->draw(platform);

	for (auto coll : playerOne.collisionBoxes)
	{
		if (coll->GetDrawable()) window->draw(*coll);
	}
	for (auto coll : playerTwo.collisionBoxes)
	{
		if (coll->GetDrawable()) window->draw(*coll);
	}

	//if(playerOne.bodyColl->ShouldBeDrawn()) window->draw(*playerOne.bodyColl);
	//if(playerTwo.bodyColl->ShouldBeDrawn()) window->draw(*playerTwo.bodyColl);

	if (playerOne.IsActive()) window->draw(playerOne);
	if (playerTwo.IsActive()) window->draw(playerTwo);

	// Render font
	window->draw(DebugText);


}

void Scene_OfflineMatch::OverrideUpdate(float dt)
{
	playerOne.Update(dt, window);
	playerTwo.Update(dt, window);

	// Iterate all collision boxes for both players
	for (CollisionBox* collA : playerOne.collisionBoxes)
	{
		if (!collA->GetActive()) break; // break if collider is unactive

		for (CollisionBox* collB : playerTwo.collisionBoxes)
		{
			if (!collA->GetActive()) break;

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

void Scene_OfflineMatch::OverrideHandleInput(float dt)
{
	if (input->isKeyDown(sf::Keyboard::Escape)) {
		input->SetKeyUp(sf::Keyboard::Escape);
		window->close();
	}

	playerOne.HandleInput(input, dt);
	playerTwo.HandleInput(input, dt);


}
