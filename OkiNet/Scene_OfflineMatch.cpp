#include "Scene_OfflineMatch.h"

void Scene_OfflineMatch::Init()
{
	rectShape.setSize(sf::Vector2f(50, 50));
	rectShape.setPosition(0, 0);
	rectShape.setFillColor(sf::Color::Green);


}

void Scene_OfflineMatch::OverrideRender()
{
	// render something to test
	window->draw(rectShape);
}

void Scene_OfflineMatch::OverrideUpdate(float dt)
{
}

void Scene_OfflineMatch::OverrideHandleInput(float dt)
{
}
