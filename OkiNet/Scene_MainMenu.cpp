#include "Scene_MainMenu.h"

void Scene_MainMenu::Init(GameState* stateMan)
{
	stateManager = stateMan;
	currentSelection = menuSelection::OfflineMatch;
	currentOnlineSelection = onlineSelection::Join;
	currentHostSelection = hostSelection::NONE;

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
	hostText.setPosition(sf::Vector2f(200, 90));
	hostText.setFillColor(sf::Color::Black);

	joinText.setFont(menuFont);
	joinText.setString("JOIN");
	joinText.setCharacterSize(20);
	joinText.setOrigin(sf::Vector2f(onlineMatchText.getGlobalBounds().width / 2, onlineMatchText.getGlobalBounds().height / 2));
	joinText.setPosition(sf::Vector2f(0, 90));
	joinText.setFillColor(sf::Color::Black);

	ipBox.setOrigin(sf::Vector2f(onlineMatchText.getGlobalBounds().width / 2, onlineMatchText.getGlobalBounds().height / 2));
	ipBox.setPosition(sf::Vector2f(-5, 120));
	ipBox.setSize(sf::Vector2f(200, 25));
	ipBox.setFillColor(sf::Color::Transparent);

	ipString = "Enter IP";
	ipText.setFont(menuFont);
	ipText.setString(ipString);
	ipText.setCharacterSize(20);
	ipText.setOrigin(sf::Vector2f(onlineMatchText.getGlobalBounds().width / 2, onlineMatchText.getGlobalBounds().height / 2));
	ipText.setPosition(sf::Vector2f(0, 120));
	ipText.setFillColor(sf::Color::Transparent);

	portBox.setOrigin(sf::Vector2f(onlineMatchText.getGlobalBounds().width / 2, onlineMatchText.getGlobalBounds().height / 2));
	portBox.setPosition(sf::Vector2f(-5, 150));
	portBox.setSize(sf::Vector2f(200, 25));
	portBox.setFillColor(sf::Color::Transparent);

	portString = "Enter Port";
	portText.setFont(menuFont);
	portText.setString(portString);
	portText.setCharacterSize(20);
	portText.setOrigin(sf::Vector2f(onlineMatchText.getGlobalBounds().width / 2, onlineMatchText.getGlobalBounds().height / 2));
	portText.setPosition(sf::Vector2f(0, 150));
	portText.setFillColor(sf::Color::Transparent);


}

void Scene_MainMenu::OverrideRender()
{

	// Render font
	window->draw(offlineMatchText);
	window->draw(onlineMatchText);
	window->draw(joinText);
	window->draw(hostText);

	window->draw(ipBox);
	window->draw(ipText);
	window->draw(portBox);
	window->draw(portText);


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

			if (currentHostSelection == hostSelection::IP)
			{
				ipText.setFillColor(sf::Color::Red);
				portText.setFillColor(sf::Color::Black);
				ipBox.setFillColor(sf::Color(1, 1, 1, 70));
				portBox.setFillColor(sf::Color(1, 1, 1, 70));
			}
			else if (currentHostSelection == hostSelection::PORT)
			{
				ipText.setFillColor(sf::Color::Black);
				portText.setFillColor(sf::Color::Red);
				ipBox.setFillColor(sf::Color(1, 1, 1, 70));
				portBox.setFillColor(sf::Color(1, 1, 1, 70));
			}
		}

	}
	else if (currentOnlineSelection == onlineSelection::Host)
	{
		joinText.setFillColor(sf::Color::Black);
		hostText.setFillColor(sf::Color::Red);
	}

	if (input->IsTextInputFieldActive())
	{
		if (currentHostSelection == hostSelection::IP)
		{
			ipString = input->GetCurrentInputText();
			ipText.setString(ipString);
		}
		else if (currentHostSelection == hostSelection::PORT)
		{
			portString = input->GetCurrentInputText();
			portText.setString(portString);
		}
	}

}

void Scene_MainMenu::OverrideHandleInput(float dt)
{
	if (input->IsKeyDown(sf::Keyboard::Escape)) {
		input->SetKeyUp(sf::Keyboard::Escape);
		window->close();
	}

	// Allow movement 
	if (input->IsKeyDown(sf::Keyboard::W) || input->IsKeyDown(sf::Keyboard::Up) || input->IsKeyDown(sf::Keyboard::S) || input->IsKeyDown(sf::Keyboard::Down))
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
		else if (currentSelection == menuSelection::OnlineMatch)
		{
			if (currentHostSelection != hostSelection::NONE)
			{
				if (currentHostSelection == hostSelection::IP)
				{
					currentHostSelection = hostSelection::PORT;
				}
				else if (currentHostSelection == hostSelection::PORT)
				{
					currentHostSelection = hostSelection::IP;
				}
			}
			else currentSelection = menuSelection::OfflineMatch;

		}


	}

	if (currentSelection == menuSelection::OnlineMatch && input->IsKeyDown(sf::Keyboard::A) || input->IsKeyDown(sf::Keyboard::Left) || input->IsKeyDown(sf::Keyboard::D) || input->IsKeyDown(sf::Keyboard::Right))
	{
		// Lift buttons so it acts as a trigger instead of constant press
		input->SetKeyUp(sf::Keyboard::A);
		input->SetKeyUp(sf::Keyboard::Right);
		input->SetKeyUp(sf::Keyboard::D);
		input->SetKeyUp(sf::Keyboard::Left);

		if (currentHostSelection == hostSelection::NONE)
		{
			if (currentOnlineSelection == onlineSelection::Host)
			{
				currentOnlineSelection = onlineSelection::Join;
			}
			else currentOnlineSelection = onlineSelection::Host;
		}


	}

	// Allow selection
	if (input->IsKeyDown(sf::Keyboard::Return))
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
				if (!input->IsTextInputFieldActive())
				{
					input->ClearInputField();
					input->SetTextInputFieldActive(true);
				}
				currentHostSelection = hostSelection::IP;
			}

		}
	}

}
