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
}

void Scene_OnlineMatch::OverrideUpdate(float dt)
{
	remotePlayerConnected = thisPeer->IsConnected();
	
	// Update players, if connection fails go back to menu
	localPlayer.Update(dt, window);
	if (remotePlayerConnected) remotePlayer.Update(dt, window);

	// Iterate all current's frame collision boxes for both players
	for (auto collA : localPlayer.GetCurrentCollision())
	{
		for (auto collB : remotePlayer.GetCurrentCollision())
		{
			if (collA->IsActive() && collB->IsActive())
			{
				Collision::CollisionResponse newColl = Collision::checkBoundingBox_Sides(collA, collB);

				if (newColl.None) // If there was no collision...
				{
					localPlayer.NoCollisionRegistered();
					remotePlayer.NoCollisionRegistered();

					DebugText.setString("NO COLLISION");
				}
				else
				{
					localPlayer.CollisionResponseToPlayer(&newColl);
					remotePlayer.CollisionResponseToPlayer(&newColl);

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

	localPlayer.HandleInput(input, dt);
	//remotePlayer.HandleInput(input, dt); // Remote player wont be accepting input
}
