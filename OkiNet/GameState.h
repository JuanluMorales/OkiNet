#pragma once
#include "Scene.h"

// Class that manages the state of the scenes in the game
enum class scenes
{
	MainMenu,
	OfflineMatch,
	OnlineMatchClient,
	OnlineMatchHost
};

class GameState
{
public:
	GameState(scenes CurrentSceneReference);
	void GoToScene(scenes sceneRef);

	scenes currentScene;
};

