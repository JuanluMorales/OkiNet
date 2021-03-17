#include "Scene_MainMenu.h"

void Scene_MainMenu::Init(GameState* stateMan)
{
	stateManager = stateMan;
	currentSelection = menuSelection::OfflineMatch;
	currentOnlineSelection = onlineSelection::Host;

	// Setup debug font and text
	menuFont.loadFromFile("font/8-BIT WONDER.ttf");

	offlineMatchText.setFont(menuFont);
	offlineMatchText.setString("OFFLINE MATCH");
	offlineMatchText.setCharacterSize(25);
	offlineMatchText.setOrigin(sf::Vector2f(offlineMatchText.getGlobalBounds().width / 2, offlineMatchText.getGlobalBounds().height / 2));
	offlineMatchText.setPosition(sf::Vector2f(0, -20));
	offlineMatchText.setFillColor(sf::Color::Black);

	onlineMatchText.setFont(menuFont);
	onlineMatchText.setString("ONLINE MATCH");
	onlineMatchText.setCharacterSize(25);
	onlineMatchText.setOrigin(sf::Vector2f(onlineMatchText.getGlobalBounds().width / 2, onlineMatchText.getGlobalBounds().height / 2));
	onlineMatchText.setPosition(sf::Vector2f(0, 50));
	onlineMatchText.setFillColor(sf::Color::Black);

	hostText.setFont(menuFont);
	hostText.setString("HOST");
	hostText.setCharacterSize(20);
	hostText.setOrigin(sf::Vector2f(onlineMatchText.getGlobalBounds().width / 2, onlineMatchText.getGlobalBounds().height / 2));
	hostText.setPosition(sf::Vector2f(0, 90));
	hostText.setFillColor(sf::Color::Black);

	joinText.setFont(menuFont);
	joinText.setString("JOIN");
	joinText.setCharacterSize(20);
	joinText.setOrigin(sf::Vector2f(onlineMatchText.getGlobalBounds().width / 2, onlineMatchText.getGlobalBounds().height / 2));
	joinText.setPosition(sf::Vector2f(200, 90));
	joinText.setFillColor(sf::Color::Black);
}

void Scene_MainMenu::OverrideRender()
{

	// Render font
	window->draw(offlineMatchText);
	window->draw(onlineMatchText);
	window->draw(joinText);
	window->draw(hostText);

}

void Scene_MainMenu::OverrideUpdate(float dt)
{
	// Update the buttons
	if (currentSelection == menuSelection::OfflineMatch)
	{
		offlineMatchText.setFillColor(sf::Color::Red);
		onlineMatchText.setFillColor(sf::Color::Black);
		joinText.setFillColor(sf::Color::White);
		hostText.setFillColor(sf::Color::White);
	}
	else if (currentSelection == menuSelection::OnlineMatch)
	{
		offlineMatchText.setFillColor(sf::Color::Black);
		onlineMatchText.setFillColor(sf::Color::Red);

		if (currentOnlineSelection == onlineSelection::Join)
		{
			joinText.setFillColor(sf::Color::Red);
			hostText.setFillColor(sf::Color::Black);

		}
		else if(currentOnlineSelection == onlineSelection::Host)
		{
			joinText.setFillColor(sf::Color::Black);
			hostText.setFillColor(sf::Color::Red);
		}

	}

}

void Scene_MainMenu::OverrideHandleInput(float dt)
{
	if (input->isKeyDown(sf::Keyboard::Escape)) {
		input->SetKeyUp(sf::Keyboard::Escape);
		window->close();
	}

	// Allow movement 
	if (input->isKeyDown(sf::Keyboard::W) || input->isKeyDown(sf::Keyboard::Up) || input->isKeyDown(sf::Keyboard::S) || input->isKeyDown(sf::Keyboard::Down))
	{
		// Lift buttons so it acts as a trigger instead of constant press
		input->SetKeyUp(sf::Keyboard::W);
		input->SetKeyUp(sf::Keyboard::Up);
		input->SetKeyUp(sf::Keyboard::S);
		input->SetKeyUp(sf::Keyboard::Down);

		if (currentSelection == menuSelection::OfflineMatch)
		{
			currentSelection = menuSelection::OnlineMatch;
		}
		else
		{
			currentSelection = menuSelection::OfflineMatch;
		}


	}

	if (currentSelection == menuSelection::OnlineMatch && input->isKeyDown(sf::Keyboard::A) || input->isKeyDown(sf::Keyboard::Left) || input->isKeyDown(sf::Keyboard::D) || input->isKeyDown(sf::Keyboard::Right))
	{
		// Lift buttons so it acts as a trigger instead of constant press
		input->SetKeyUp(sf::Keyboard::A);
		input->SetKeyUp(sf::Keyboard::Right);
		input->SetKeyUp(sf::Keyboard::D);
		input->SetKeyUp(sf::Keyboard::Left);

		if (currentOnlineSelection == onlineSelection::Host)
		{
			currentOnlineSelection = onlineSelection::Join;
		}
		else currentOnlineSelection = onlineSelection::Host;

	}

	// Allow selection
	if (input->isKeyDown(sf::Keyboard::Return))
	{
		input->SetKeyUp(sf::Keyboard::Return);

		if (currentSelection == menuSelection::OfflineMatch)
		{
			stateManager->GoToScene(scenes::OfflineMatch);
		}
		else
		{
			// TODO: INDICATE HOST OR JOIN TO THE SCENE + ADD INPUT FOR IP AND PORT TO HOST SELECTION
			if (currentOnlineSelection == onlineSelection::Host)
			{
				stateManager->GoToScene(scenes::OnlineMatch);
			}
			else if (currentOnlineSelection == onlineSelection::Join)
			{
				stateManager->GoToScene(scenes::OnlineMatch);
			}

		}
	}

}
