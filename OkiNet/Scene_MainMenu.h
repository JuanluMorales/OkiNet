#pragma once
#include "GameState.h"

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
private:
	GameState* stateManager;

	sf::Font menuFont;
	sf::Text onlineMatchText;
	sf::Text offlineMatchText;
	sf::Text hostText;
	sf::Text joinText;

	menuSelection currentSelection;
	onlineSelection currentOnlineSelection;
};

