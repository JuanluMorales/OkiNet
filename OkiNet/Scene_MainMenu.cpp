#include "Scene_MainMenu.h"

void Scene_MainMenu::Init(GameState* stateMan)
{
	stateManager = stateMan;
	currentSelection = menuSelection::OfflineMatch;

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
}

void Scene_MainMenu::OverrideRender()
{

	// Render font
	window->draw(offlineMatchText);
	window->draw(onlineMatchText);


}

void Scene_MainMenu::OverrideUpdate(float dt)
{
	// Update the buttons
	if (currentSelection == menuSelection::OfflineMatch) 
	{
		offlineMatchText.setFillColor(sf::Color::Red);
		onlineMatchText.setFillColor(sf::Color::Black);
	}else
	{
		offlineMatchText.setFillColor(sf::Color::Black);
		onlineMatchText.setFillColor(sf::Color::Red);
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
		else currentSelection = menuSelection::OfflineMatch;

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
			stateManager->GoToScene(scenes::OnlineMatch);
		}
	}

}
