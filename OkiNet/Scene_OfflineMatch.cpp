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

	// Environment collision
	leftColl = new CollisionBox(CollisionBox::ColliderType::Environment, sf::Color::Transparent,sf::Color::Green,2.0f, sf::Vector2f(-750, 0), sf::Vector2f(250, 500), sf::Vector2f(0, 0));
	rightColl = new CollisionBox(CollisionBox::ColliderType::Environment, sf::Color::Transparent, sf::Color::Green, 2.0f, sf::Vector2f(695, 0), sf::Vector2f(250, 500), sf::Vector2f(0, 0));

	//Player 1 lifebar
	int disp = 0;
	sf::Vector2f startPos = sf::Vector2f(-100, -350);
	sf::Vector2f unitSize = sf::Vector2f(5, 30);
	for (int i = 0; i < 100; i++)
	{
		sf::RectangleShape* lifeBarStack = new sf::RectangleShape();
		lifeBarStack->setFillColor(sf::Color::Green);
		lifeBarStack->setPosition(sf::Vector2f(startPos.x + disp, startPos.y));
		lifeBarStack->setSize(unitSize);
		p1_lifeBar.push_back(lifeBarStack);

		sf::RectangleShape* lifeBarBackgroundStack = new sf::RectangleShape();
		lifeBarBackgroundStack->setFillColor(sf::Color::Black);
		lifeBarBackgroundStack->setPosition(sf::Vector2f((startPos.x - 7.0f) + disp, startPos.y - 6.0f));
		lifeBarBackgroundStack->setSize(sf::Vector2f(unitSize.x * 4.0f, unitSize.y * 1.4f));
		p1_lifeBarBackground.push_back(lifeBarBackgroundStack);
		disp -= static_cast<int>(unitSize.x);
	}
	// Player 2
	disp = 0;
	startPos = sf::Vector2f(100, -350);
	unitSize = sf::Vector2f(5, 30);
	for (int i = 0; i < 100; i++)
	{
		sf::RectangleShape* lifeBarStack = new sf::RectangleShape();
		lifeBarStack->setFillColor(sf::Color::Green);
		lifeBarStack->setPosition(sf::Vector2f(startPos.x + disp, startPos.y));
		lifeBarStack->setSize(unitSize);
		p2_lifeBar.push_back(lifeBarStack);

		sf::RectangleShape* lifeBarBackgroundStack = new sf::RectangleShape();
		lifeBarBackgroundStack->setFillColor(sf::Color::Black);
		lifeBarBackgroundStack->setPosition(sf::Vector2f((startPos.x - 7.0f) + disp, startPos.y - 6.0f));
		lifeBarBackgroundStack->setSize(sf::Vector2f(unitSize.x * 4.0f, unitSize.y * 1.4f));
		p2_lifeBarBackground.push_back(lifeBarBackgroundStack);
		disp += static_cast<int>(unitSize.x);
	}

}

void Scene_OfflineMatch::OverrideEarlyUpdate(float dt)
{
}

void Scene_OfflineMatch::OverrideRender()
{
	// draw players
	if (playerOne.IsActive()) window->draw(playerOne);
	if (playerTwo.IsActive()) window->draw(playerTwo);

	window->draw(*leftColl);
	window->draw(*rightColl);

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

	// Draw UI
	for (auto hpBack : p1_lifeBarBackground)
	{
		window->draw(*hpBack);
	}
	for (auto hp : p1_lifeBar)
	{
		window->draw(*hp);
	}
	for (auto hpBack : p2_lifeBarBackground)
	{
		window->draw(*hpBack);
	}
	for (auto hp : p2_lifeBar)
	{
		window->draw(*hp);
	}

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
				// Check collision between players
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

				// Check collision between the players and the map limits
				Collision::CollisionResponse newColl2 = Collision::checkBoundingBox_Sides(collA, leftColl);
				if (newColl2.None)
				{
					playerOne.CanGoLeft = true;
				}
				else
				{
					playerOne.CanGoLeft = false;
				}

				Collision::CollisionResponse newColl3 = Collision::checkBoundingBox_Sides(collB, rightColl);
				if (newColl3.None)
				{
					playerTwo.CanGoRight = true;
				}
				else
				{
					playerTwo.CanGoRight = false;
				}
			}
		}
	}


}

void Scene_OfflineMatch::OverrideHandleInput(float dt)
{
	if (input->IsKeyDown(sf::Keyboard::Escape)) {
		input->SetKeyUp(sf::Keyboard::Escape);
		window->close();
	}

	playerOne.HandleInput(input, dt);
	playerTwo.HandleInput(input, dt);


}
