#include <SFML/Graphics.hpp>
#include "InputManager.h"
#include "GameState.h" // Includes Scene.h and other scenes

// Scenes includes
#include "Scene_MainMenu.h"
#include "Scene_OfflineMatch.h"
#include "Scene_OnlineMatch.h"

// Aspect ratio of the window
const unsigned int WindowWidth = 1280;
const unsigned int WindowHeight = 720;

const char* WindowName = "OkiNet";

const unsigned int FrameRateCap = 60;

int main()
{
	// Create components needed for SFML to run
	// Window to render on
	sf::RenderWindow window(sf::VideoMode(WindowWidth, WindowHeight), WindowName);
	window.setFramerateLimit(FrameRateCap);
	// View (center, size) --> Acts as a "2D camera" or viewport
	sf::View view(sf::Vector2f(0, 0), sf::Vector2f(WindowWidth, WindowHeight));
	// Input
	InputManager inputManager;
	// Clock with delta time
	sf::Clock clock;
	float deltaTime;

	// Game Scene Manager
	GameState* scenesManager;
	scenesManager = new GameState(scenes::MainMenu);

	// Create first scene: main menu so it can be fed to the sceneManager
	Scene_MainMenu mainMenu(&window, &inputManager, &view);
	mainMenu.Init(scenesManager);

	// Create and initialize offline test scene
	Scene_OfflineMatch offlineMatch(&window, &inputManager, &view);
	offlineMatch.Init(scenesManager);

	Scene_OnlineMatch onlineMatch(&window, &inputManager, &view);
	onlineMatch.Init(scenesManager);


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

		// FakeSwitch on the scenes to select which one to update
		if (scenesManager->currentScene == scenes::MainMenu)
		{
			mainMenu.RunScene(deltaTime);
		}else
		if (scenesManager->currentScene == scenes::OfflineMatch)
		{
			offlineMatch.RunScene(deltaTime);
		}else if (scenesManager->currentScene == scenes::OnlineMatch)
		{
			onlineMatch.RunScene(deltaTime);
		}
		
	

	}

	return 0;
}


