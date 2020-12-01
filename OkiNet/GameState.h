#pragma once
#include "Scene.h"

// Scenes includes
#include "Scene_MainMenu.h"
#include "Scene_OfflineMatch.h"


// Class that manages the state of the scenes in the game
class GameState
{
public:
	GameState(Scene* CurrentSceneReference);
	Scene* GoToScene(Scene* sceneRef);

	Scene* currentScene;
};

