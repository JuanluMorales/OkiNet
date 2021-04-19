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

	// Setup initialization for the host and joiner
	playerOneStartPos = sf::Vector2f(-200, 0);
	playerTwoStartPos = sf::Vector2f(200, 0);
	if (isHost)
	{
		localPlayer.InitNetworkedCharacter(PlayerCharacter::PlayerID::PlayerOne, playerOneStartPos, thisPeer, true);
		remotePlayer.InitNetworkedCharacter(PlayerCharacter::PlayerID::PlayerTwo, playerTwoStartPos, thisPeer, false);
	}
	else
	{
		localPlayer.InitNetworkedCharacter(PlayerCharacter::PlayerID::PlayerTwo, playerTwoStartPos, thisPeer, true);
		remotePlayer.InitNetworkedCharacter(PlayerCharacter::PlayerID::PlayerOne, playerOneStartPos, thisPeer, false);
	}


	// Environment collision
	leftColl = new CollisionBox(CollisionBox::ColliderType::Environment, sf::Color::Transparent, sf::Color::Green, 2.0f, sf::Vector2f(-750, 0), sf::Vector2f(250, 500), sf::Vector2f(0, 0));
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

	//Player 1 energy bar
	disp = 0;
	startPos = sf::Vector2f(-99, -315);
	unitSize = sf::Vector2f(4, 15);
	for (int i = 0; i < 100; i++)
	{
		sf::RectangleShape* energyBarStack = new sf::RectangleShape();
		energyBarStack->setFillColor(sf::Color::Yellow);
		energyBarStack->setPosition(sf::Vector2f(startPos.x + disp, startPos.y));
		energyBarStack->setSize(unitSize);
		p1_energyBar.push_back(energyBarStack);

		sf::RectangleShape* energyBarBackgroundStack = new sf::RectangleShape();
		energyBarBackgroundStack->setFillColor(sf::Color::Black);
		energyBarBackgroundStack->setPosition(sf::Vector2f((startPos.x - 4.0f) + disp, startPos.y - 3.0f));
		energyBarBackgroundStack->setSize(sf::Vector2f(unitSize.x * 4.0f, unitSize.y * 1.4f));
		p1_energyBarBackground.push_back(energyBarBackgroundStack);
		disp -= static_cast<int>(unitSize.x);
	}
	// Player 2
	disp = 0;
	startPos = sf::Vector2f(100, -315);
	unitSize = sf::Vector2f(4, 15);
	for (int i = 0; i < 100; i++)
	{
		sf::RectangleShape* energyBarStack = new sf::RectangleShape();
		energyBarStack->setFillColor(sf::Color::Yellow);
		energyBarStack->setPosition(sf::Vector2f(startPos.x + disp, startPos.y));
		energyBarStack->setSize(unitSize);
		p2_energyBar.push_back(energyBarStack);

		sf::RectangleShape* energyBarBackgroundStack = new sf::RectangleShape();
		energyBarBackgroundStack->setFillColor(sf::Color::Black);
		energyBarBackgroundStack->setPosition(sf::Vector2f((startPos.x - 7.0f) + disp, startPos.y - 3.0f));
		energyBarBackgroundStack->setSize(sf::Vector2f(unitSize.x * 4.0f, unitSize.y * 1.4f));
		p2_energyBarBackground.push_back(energyBarBackgroundStack);
		disp += static_cast<int>(unitSize.x);
	}

	// Score texts
	p1ScoreText.setFont(DebugFont);
	p2ScoreText.setFont(DebugFont);
	p1ScoreText.setString("0");
	p2ScoreText.setString("0");
	p1ScoreText.setOrigin(sf::Vector2f(DebugText.getGlobalBounds().width / 2, DebugText.getGlobalBounds().height / 2));
	p1ScoreText.setPosition(sf::Vector2f(0, -300));
	p1ScoreText.setFillColor(sf::Color::Black);
	p2ScoreText.setOrigin(sf::Vector2f(DebugText.getGlobalBounds().width / 2, DebugText.getGlobalBounds().height / 2));
	p2ScoreText.setPosition(sf::Vector2f(250, -300));
	p2ScoreText.setFillColor(sf::Color::Black);
	p1ScoreText.setCharacterSize(55);
	p2ScoreText.setCharacterSize(55);

}

void Scene_OnlineMatch::InitAsHost(GameState* stateMan, std::string& port)
{
	isHost = true;

	int strPort = std::stoi(port); // convert string to int
	thisPeer = std::make_shared<NetworkPeer>(strPort);
	thisPeer->StartListening();

	Init(stateMan); // Initialize normally
}

void Scene_OnlineMatch::InitAsClient(GameState* stateMan, std::string& ip, std::string& port)
{
	isHost = false;

	bool connSuccesful = false;
	int strPort = std::stoi(port); // convert string to int
	// Attempt connection
	thisPeer = std::make_shared<NetworkPeer>(strPort);
	connSuccesful = thisPeer->Connect(ip, strPort);

	// if failed go back to main menu
	if (!connSuccesful) stateMan->GoToScene(scenes::MainMenu);

	remotePlayerConnected = connSuccesful;

	Init(stateMan); // Initialize normally
}

void Scene_OnlineMatch::OverrideRender()
{
	// draw players
	if (isHost)
	{
		for (auto coll : localPlayer.GetCurrentCollision())
		{
			if (coll->GetDrawable() && coll->IsActive()) window->draw(*coll);
		}

		if (localPlayer.IsActive()) window->draw(localPlayer);

		if (remotePlayerConnected)
		{
			for (auto coll : remotePlayer.GetCurrentCollision())
			{
				if (coll->GetDrawable() && coll->IsActive()) window->draw(*coll);
			}
			if (remotePlayer.IsActive()) window->draw(remotePlayer);
		}
	}
	else
	{
		// draw player collision
		for (auto coll : localPlayer.GetCurrentCollision())
		{
			if (coll->GetDrawable() && coll->IsActive()) window->draw(*coll);
		}
		for (auto coll : remotePlayer.GetCurrentCollision())
		{
			if (coll->GetDrawable() && coll->IsActive()) window->draw(*coll);
		}

		// Draw players
		if (localPlayer.IsActive()) window->draw(localPlayer);
		if (remotePlayer.IsActive()) window->draw(remotePlayer);
	}


	// Render font
	window->draw(DebugText);
	if (isHost)
	{
		window->draw(p1ScoreText);
		window->draw(p2ScoreText);
	}
	else
	{
		window->draw(p2ScoreText);
		window->draw(p1ScoreText);
	}



	// Draw UI
	// HP bars
	if (isHost)
	{
		for (auto hp : p1_lifeBarBackground)
		{
			window->draw(*hp);
		}
		for (int i = 0; i < localPlayer.currentHealthPoints; i++)
		{
			window->draw(*p1_lifeBar.at(i));
		}

		for (auto hp : p2_lifeBarBackground)
		{
			window->draw(*hp);
		}
		for (int i = 0; i < remotePlayer.currentHealthPoints; i++)
		{
			window->draw(*p2_lifeBar.at(i));
		}
		// Energy bars
		for (auto hp : p1_energyBarBackground)
		{
			window->draw(*hp);
		}
		for (int i = 0; i < localPlayer.currentEnergyPoints; i++)
		{
			window->draw(*p1_energyBar.at(i));
		}

		for (auto hp : p2_energyBarBackground)
		{
			window->draw(*hp);
		}
		for (int i = 0; i < remotePlayer.currentEnergyPoints; i++)
		{
			window->draw(*p2_energyBar.at(i));
		}
	}
	else
	{
		for (auto hp : p1_lifeBarBackground)
		{
			window->draw(*hp);
		}
		for (int i = 0; i < remotePlayer.currentHealthPoints; i++)
		{
			window->draw(*p1_lifeBar.at(i));
		}

		for (auto hp : p2_lifeBarBackground)
		{
			window->draw(*hp);
		}
		for (int i = 0; i < localPlayer.currentHealthPoints; i++)
		{
			window->draw(*p2_lifeBar.at(i));
		}
		// Energy bars
		for (auto hp : p1_energyBarBackground)
		{
			window->draw(*hp);
		}
		for (int i = 0; i < remotePlayer.currentEnergyPoints; i++)
		{
			window->draw(*p1_energyBar.at(i));
		}

		for (auto hp : p2_energyBarBackground)
		{
			window->draw(*hp);
		}
		for (int i = 0; i < localPlayer.currentEnergyPoints; i++)
		{
			window->draw(*p2_energyBar.at(i));
		}
	}
	
}

void Scene_OnlineMatch::OverrideUpdate(float dt)
{
	if (localPlayer.currentHealthPoints <= 0 || remotePlayer.currentHealthPoints <= 0)
	{
		if (restartCounter >= restartTime) Restart();
		restartCounter++;
	}

	remotePlayerConnected = thisPeer->IsConnected();
	
	// Update players, if connection fails go back to menu
	localPlayer.Update(dt, window);
	if (remotePlayerConnected) remotePlayer.Update(dt, window);

	// Update score text
	p1ScoreText.setString(std::to_string(p1Score));
	p2ScoreText.setString(std::to_string(p2Score));

	// Update collisions for both users
	if (isHost)
	{
		// Iterate all current's frame collision boxes for both players
		bool interPlayerCollision = false; // Bool to check if there was a collision between players at all so that damage is only applied once per collision
		for (auto collA : localPlayer.GetCurrentCollision())
		{
			for (auto collB : remotePlayer.GetCurrentCollision())
			{
				if (collA->IsActive() && collB->IsActive())
				{
					// Check collision between players
					Collision::CollisionResponse newColl = Collision::checkBoundingBox_Sides(collA, collB, localPlayer.GetCurrentAnimation(), remotePlayer.GetCurrentAnimation());

					if (!newColl.None) // There was no collision between the checked colliders (but could be a collision with future colliders
					{
						interPlayerCollision = true;
						localPlayer.CollisionResponseToPlayer(&newColl);
						remotePlayer.CollisionResponseToPlayer(&newColl);
					}
				}
			}
		}


		if (!interPlayerCollision)
		{
			localPlayer.NoCollisionRegistered();
			remotePlayer.NoCollisionRegistered();

			DebugText.setString("NO COLLISION");
		}

		// Check map collision
		for (auto collA : localPlayer.GetCurrentCollision())
		{
			for (auto collB : remotePlayer.GetCurrentCollision())
			{
				// Check collision between the players and the map limits
				Collision::CollisionResponse newColl2 = Collision::checkBoundingBox_Sides(collA, leftColl);
				if (newColl2.None)
				{
					localPlayer.CanGoLeft = true;
				}
				else
				{
					localPlayer.CanGoLeft = false;
				}

				Collision::CollisionResponse newColl3 = Collision::checkBoundingBox_Sides(collB, rightColl);
				if (newColl3.None)
				{
					remotePlayer.CanGoRight = true;
				}
				else
				{
					remotePlayer.CanGoRight = false;
				}
			}
		}
	}
	else
	{
		// Iterate all current's frame collision boxes for both players
		bool interPlayerCollision = false; // Bool to check if there was a collision between players at all so that damage is only applied once per collision
		for (auto collA : remotePlayer.GetCurrentCollision())
		{
			for (auto collB : localPlayer.GetCurrentCollision())
			{
				if (collA->IsActive() && collB->IsActive())
				{
					// Check collision between players
					Collision::CollisionResponse newColl = Collision::checkBoundingBox_Sides(collA, collB, remotePlayer.GetCurrentAnimation(), localPlayer.GetCurrentAnimation());

					if (!newColl.None) // There was no collision between the checked colliders (but could be a collision with future colliders
					{
						interPlayerCollision = true;
						localPlayer.CollisionResponseToPlayer(&newColl);
						remotePlayer.CollisionResponseToPlayer(&newColl);
					}
				}
			}
		}


		if (!interPlayerCollision)
		{
			localPlayer.NoCollisionRegistered();
			remotePlayer.NoCollisionRegistered();

			DebugText.setString("NO COLLISION");
		}

		// Check map collision
		for (auto collA : remotePlayer.GetCurrentCollision())
		{
			for (auto collB : localPlayer.GetCurrentCollision())
			{
				// Check collision between the players and the map limits
				Collision::CollisionResponse newColl2 = Collision::checkBoundingBox_Sides(collA, leftColl);
				if (newColl2.None)
				{
					remotePlayer.CanGoLeft = true;
				}
				else
				{
					remotePlayer.CanGoLeft = false;
				}

				Collision::CollisionResponse newColl3 = Collision::checkBoundingBox_Sides(collB, rightColl);
				if (newColl3.None)
				{
					localPlayer.CanGoRight = true;
				}
				else
				{
					localPlayer.CanGoRight = false;
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

	// Dont accept input until the remote is ready -> potential critical desync issue
	if (remotePlayerConnected)
	{
		localPlayer.HandleInput(input, dt);
		remotePlayer.HandleRemotePlayerInput(input, dt); // Remote player wont be accepting input
	}
}

void Scene_OnlineMatch::Restart()
{
	thisMatchState = MatchState::Restart;
	if (isHost)
	{
		localPlayer.setPosition(playerOneStartPos);
		remotePlayer.setPosition(playerTwoStartPos);
	}
	else 
	{
		localPlayer.setPosition(playerTwoStartPos);
		remotePlayer.setPosition(playerOneStartPos);
	}

	localPlayer.currentHealthPoints = 100;
	remotePlayer.currentHealthPoints = 100;
	localPlayer.currentEnergyPoints = 100;
	remotePlayer.currentEnergyPoints = 100;
	thisMatchState = MatchState::Start;

	if (localPlayer.currentHealthPoints <= 0) {
		p2Score += 1; 
	}
	else p1Score += 1;

	restartCounter = 0;
}