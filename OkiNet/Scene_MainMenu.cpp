#include "Scene_MainMenu.h"

void Scene_MainMenu::Init(GameState* stateMan)
{
	stateManager = stateMan;
	currentSelection = menuSelection::OfflineMatch;
	currentOnlineSelection = onlineSelection::Join;
	currentHostSelection = hostSelection::NONE;

	// Setup debug font and text
	menuFont.loadFromFile("font/8-BIT WONDER.ttf");
	inputFont.loadFromFile("font/OpenSans-Regular.ttf");

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

	ipString = "";
	ipText.setFont(inputFont);
	ipText.setString(ipString);
	ipText.setCharacterSize(20);
	ipText.setOrigin(sf::Vector2f(onlineMatchText.getGlobalBounds().width / 2, onlineMatchText.getGlobalBounds().height / 2));
	ipText.setPosition(sf::Vector2f(0, 120));
	ipText.setFillColor(sf::Color::Transparent);

	portBox.setOrigin(sf::Vector2f(onlineMatchText.getGlobalBounds().width / 2, onlineMatchText.getGlobalBounds().height / 2));
	portBox.setPosition(sf::Vector2f(-5, 150));
	portBox.setSize(sf::Vector2f(200, 25));
	portBox.setFillColor(sf::Color::Transparent);

	portString = "";
	portText.setFont(inputFont);
	portText.setString(portString);
	portText.setCharacterSize(20);
	portText.setOrigin(sf::Vector2f(onlineMatchText.getGlobalBounds().width / 2, onlineMatchText.getGlobalBounds().height / 2));
	portText.setPosition(sf::Vector2f(0, 150));
	portText.setFillColor(sf::Color::Transparent);

	ipTextOverlay.setFont(menuFont);
	ipTextOverlay.setString("Enter IP");
	ipTextOverlay.setCharacterSize(20);
	ipTextOverlay.setOrigin(sf::Vector2f(onlineMatchText.getGlobalBounds().width / 2, onlineMatchText.getGlobalBounds().height / 2));
	ipTextOverlay.setPosition(sf::Vector2f(-200, 120));
	ipTextOverlay.setFillColor(sf::Color::Transparent);

	portTextOverlay.setFont(menuFont);
	portTextOverlay.setString("Enter PORT");
	portTextOverlay.setCharacterSize(20);
	portTextOverlay.setOrigin(sf::Vector2f(onlineMatchText.getGlobalBounds().width / 2, onlineMatchText.getGlobalBounds().height / 2));
	portTextOverlay.setPosition(sf::Vector2f(-200, 150));
	portTextOverlay.setFillColor(sf::Color::Transparent);


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
	window->draw(ipTextOverlay);
	window->draw(portTextOverlay);


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
		ipTextOverlay.setFillColor(sf::Color::Transparent);
		portTextOverlay.setFillColor(sf::Color::Transparent);
	}
	if (currentSelection == menuSelection::OnlineMatch)
	{
		offlineMatchText.setFillColor(sf::Color::Black);
		onlineMatchText.setFillColor(sf::Color::Red);

		ipTextOverlay.setFillColor(sf::Color::Transparent);
		portTextOverlay.setFillColor(sf::Color::Transparent);

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
				ipTextOverlay.setFillColor(sf::Color::Black);
				portTextOverlay.setFillColor(sf::Color::Black);
			}
			else if (currentHostSelection == hostSelection::PORT)
			{
				ipText.setFillColor(sf::Color::Black);
				portText.setFillColor(sf::Color::Red);
				ipBox.setFillColor(sf::Color(1, 1, 1, 70));
				portBox.setFillColor(sf::Color(1, 1, 1, 70));
				ipTextOverlay.setFillColor(sf::Color::Black);
				portTextOverlay.setFillColor(sf::Color::Black);
			}
		}else if (currentOnlineSelection == onlineSelection::Host)
		{
			joinText.setFillColor(sf::Color::Black);
			hostText.setFillColor(sf::Color::Red);

			if (currentHostSelection == hostSelection::PORT)
			{
				portText.setFillColor(sf::Color::Red);
				portBox.setFillColor(sf::Color(1, 1, 1, 70));
				portTextOverlay.setFillColor(sf::Color::Black);
			}
		}

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
			if (currentHostSelection != hostSelection::NONE && currentOnlineSelection == onlineSelection::Join)
			{
				if (currentHostSelection == hostSelection::IP)
				{
					currentHostSelection = hostSelection::PORT;
					input->SetInputField(portString);
				}
				else if (currentHostSelection == hostSelection::PORT)
				{
					currentHostSelection = hostSelection::IP;
					input->SetInputField(ipString);
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

		if (currentHostSelection == hostSelection::NONE )
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
			if (currentOnlineSelection == onlineSelection::Host)
			{
				if (!input->IsTextInputFieldActive())
				{
					currentHostSelection = hostSelection::PORT;
					input->ClearInputField();
					input->SetTextInputFieldActive(true);
				}
				else		{

					stateManager->GoToScene(scenes::OnlineMatchHost);
					input->ClearInputField();
					input->SetTextInputFieldActive(false);
				}
			}
			else if (currentOnlineSelection == onlineSelection::Join)
			{
				if (!input->IsTextInputFieldActive())
				{
					currentHostSelection = hostSelection::IP;
					input->ClearInputField();
					input->SetTextInputFieldActive(true);
				}
				else
				{
					stateManager->GoToScene(scenes::OnlineMatchClient);
					input->ClearInputField();
					input->SetTextInputFieldActive(false);
				}
				
			}

		}
	}

}
