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
	window.setKeyRepeatEnabled(false); // Disable repeat keys so it is possible to have buttons as triggers
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
	Scene_OfflineMatch* offlineMatch = nullptr;

	// Create and initialize online scene
	Scene_OnlineMatch* onlineMatchHost = nullptr;
	Scene_OnlineMatch* onlineMatchClient = nullptr;



	// GAME EXECUTION LOOP
	while (window.isOpen())
	{
		sf::Event event;

		// Listen to window and input events 
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
				inputManager.SetMousePosition(event.mouseMove.x, event.mouseMove.y);
				break;

			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Left) {
					inputManager.SetMouseLeftDown(true);
				}
				if (event.mouseButton.button == sf::Mouse::Right) {
					inputManager.SetMouseRightDown(true);
				}
				break;

			case sf::Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Left) {
					inputManager.SetMouseLeftDown(false);
				}
				if (event.mouseButton.button == sf::Mouse::Right) {
					inputManager.SetMouseRightDown(false);
				}
				break;
			case sf::Event::TextEntered:
				if (inputManager.IsTextInputFieldActive() && event.text.unicode < 128)
				{
					const char* backspace = "\b";
					if (!inputManager.GetCurrentInputText().empty() && static_cast<char>(event.text.unicode) == *backspace)
					{
						inputManager.RemoveLetterFromInputField();
					}
					else inputManager.WriteInputField(event.text.unicode);
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

		// Switch on the scenes to select which one to update
		switch (scenesManager->currentScene)
		{
			// MAIN MENU
		case scenes::MainMenu:
			mainMenu.RunScene(deltaTime);
			break;

			// OFFLINE
		case scenes::OfflineMatch:
			// Initialize scene
			if (offlineMatch == nullptr)
			{
				offlineMatch = new Scene_OfflineMatch(&window, &inputManager, &view);
				offlineMatch->Init(scenesManager);
			}

			offlineMatch->RunScene(deltaTime);

			break;

			//ONLINE - AS HOST
		case scenes::OnlineMatchHost:
			if (onlineMatchHost == nullptr)
			{
				onlineMatchHost = new Scene_OnlineMatch(&window, &inputManager, &view);
				onlineMatchHost->InitAsHost(scenesManager);
			}
			
			onlineMatchHost->RunScene(deltaTime);
			
			break;

			//ONLINE - AS CLIENT
		case scenes::OnlineMatchClient:
			if (onlineMatchClient == nullptr)
			{
				onlineMatchClient = new Scene_OnlineMatch(&window, &inputManager, &view);
				onlineMatchClient->InitAsClient(scenesManager, mainMenu.ipString, mainMenu.portString);
			}

			onlineMatchClient->RunScene(deltaTime);
			break;
		default:
			break;
		}
	}

	return 0;
}


