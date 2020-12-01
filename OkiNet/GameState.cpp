#include "GameState.h"

GameState::GameState(scenes CurrentSceneReference)
{
	currentScene = CurrentSceneReference;
}

// Search in the map for the scene and return it
void GameState::GoToScene(scenes state)
{
	currentScene = state;
}

