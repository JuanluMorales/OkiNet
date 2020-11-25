#include "Configurations.h" // Common configurations for the game
#include <net_common.h> // Network static libraries
#include <SFML/Graphics.hpp>
#include "InputManager.h"
#include "GameState.h" // Includes Scene.h



int main()
{
//start: // Start label for goto purposes

	// Create components needed for SFML to run
	// Window to render on
	sf::RenderWindow window(sf::VideoMode(configs::WindowWidth, configs::WindowHeight), configs::WindowName);
	window.setFramerateLimit(configs::FrameRateCap);
	// View (center, size)
	sf::View view(sf::Vector2f(0, 0), sf::Vector2f(configs::WindowWidth, configs::WindowHeight));
	// Input
	InputManager inputManager;
	// Clock with delta time
	sf::Clock clock;
	float deltaTime;
	//--------------------------------------

	// Create test scene
	Scene match(&window, &inputManager, &view);
	Scene_OfflineMatch offlineMatch(&window, &inputManager, &view);
	
	////// render something to test
	//sf::CircleShape shape(100.f);
	//shape.setFillColor(sf::Color::Green);

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

		//match.RunScene(deltaTime);
		
		offlineMatch.RunScene(deltaTime);

		//window.clear();
		//window.draw(shape);
		//window.display();
	}

	return 0;
}