#pragma once
#include "GameState.h"
#include <string>

// Scene that allows the main menu to be used to select scene
class Scene_MainMenu : public Scene
{

public:
	// Make sure to allow use of the default constructor
	using Scene::Scene;

	void Init(GameState* stateMan);
	void OverrideRender();
	void OverrideUpdate(float dt);
	void OverrideHandleInput(float dt);

	enum class menuSelection { OfflineMatch, OnlineMatch };
	enum class onlineSelection { Host, Join };
	enum class hostSelection { NONE,  IP, PORT };

	std::string ipString;
	std::string portString;

private:
	GameState* stateManager;

	sf::Font menuFont;
	sf::Text onlineMatchText;
	sf::Text offlineMatchText;
	sf::Text hostText;
	sf::Text joinText;


	sf::Text ipText;
	sf::Text portText;
	sf::Text ipTextOverlay;
	sf::Text portTextOverlay;
	sf::RectangleShape ipBox;
	sf::RectangleShape portBox;


	menuSelection currentSelection;
	onlineSelection currentOnlineSelection;
	hostSelection currentHostSelection;
};

