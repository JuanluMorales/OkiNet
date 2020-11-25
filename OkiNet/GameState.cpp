#include "GameState.h"

GameState::GameState(Scene* CurrentSceneReference, GameStates* CurrentState)
{
	currentScene = CurrentSceneReference;
	currentGameState = CurrentState;
}
