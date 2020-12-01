#include <SFML/Graphics.hpp>
#include "Configurations.h" // Common configurations for the game
#include <net_common.h> // Network static libraries
#include <net_threadSafeQueue.h>
#include "InputManager.h"
#include "GameState.h" // Includes Scene.h

int main()
{
	// Create components needed for SFML to run
	// Window to render on
	sf::RenderWindow window(sf::VideoMode(configs::WindowWidth, configs::WindowHeight), configs::WindowName);
	window.setFramerateLimit(configs::FrameRateCap);
	// View (center, size) --> Acts as a "2D camera" or viewport
	sf::View view(sf::Vector2f(0, 0), sf::Vector2f(configs::WindowWidth, configs::WindowHeight));
	// Input
	InputManager inputManager;
	// Clock with delta time
	sf::Clock clock;
	float deltaTime;

	// Create and initialize test scene
	Scene_OfflineMatch offlineMatch(&window, &inputManager, &view);
	offlineMatch.Init();

	// GAME LOOP
	while (window.isOpen())
	{
		sf::Event event;

		// Listen to window events 
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed: 
				window.close();
				break;

			case sf::Event::Resized:
				window.setView(sf::View(sf::FloatRect(0, 0, (float)event.size.width, (float)event.size.height)));
				break;

			case sf::Event::KeyPressed:
				//update key pressed
				inputManager.SetKeyDown(event.key.code);
				break;

			case sf::Event::KeyReleased:
				//Update key released 
				inputManager.SetKeyUp(event.key.code);
				break;

			case sf::Event::MouseMoved:
				inputManager.setMousePosition(event.mouseMove.x, event.mouseMove.y);
				break;

			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Left) {
					inputManager.setMouseLeftDown(true);
				}
				if (event.mouseButton.button == sf::Mouse::Right) {
					inputManager.setMouseRightDown(true);
				}
				break;

			case sf::Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Left) {
					inputManager.setMouseLeftDown(false);
				}
				if (event.mouseButton.button == sf::Mouse::Right) {
					inputManager.setMouseRightDown(false);
				}
				break;

			default:
				//dont handle other events
				break;
			}
		}

		//Calculate delta time; how much time has passed since 
		//it was last calculated (in seconds) and restart the clock
		deltaTime = clock.restart().asSeconds();
		
		offlineMatch.RunScene(deltaTime);

	}

	return 0;
}


