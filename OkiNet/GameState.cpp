#include "GameState.h"

GameState::GameState(Scene* CurrentSceneReference)
{
	currentScene = CurrentSceneReference;
}

// Search in the map for the scene and return it
Scene* GameState::GoToScene(Scene* state)
{
	currentScene = state;
	return currentScene;
}

