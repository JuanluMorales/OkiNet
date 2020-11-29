#pragma once
#include "Scene.h"

// Scene that comprises the offline version of the game
class Scene_OfflineMatch : public Scene
{
public:
	// Make sure to allow use of the default constructor
	using Scene::Scene;

	void Init();
	void OverrideRender();
	void OverrideUpdate(float dt);
	void OverrideHandleInput(float dt);

private:

	sf::RectangleShape rectShape;
};

