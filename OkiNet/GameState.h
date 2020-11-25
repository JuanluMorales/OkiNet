#pragma once
#include "Scene.h"
#include "Scene_OfflineMatch.h"

// Class that manages the state of the scenes in the game
// Includes the headers to all scenes in the game

// Hold the different game states
enum class GameStates 
{
	MENU, 
	LOBBY, 
	MATCH 
};

class GameState
{
	GameState(Scene* CurrentSceneReference, GameStates* CurrentScene);

	//Scene* GoToScene(Scene* sceneRef);
	//Scene* GoToScene(GameStates* stateRef);




protected:
	Scene* currentScene;
	GameStates* currentGameState;

};

