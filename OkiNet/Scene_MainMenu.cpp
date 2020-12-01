#include "Scene_MainMenu.h"

void Scene_MainMenu::Init()
{

}

void Scene_MainMenu::OverrideRender()
{

	// Render font
	window->draw(offlineMatchText);
	window->draw(onlineMatchText);


}

void Scene_MainMenu::OverrideUpdate(float dt)
{
}

void Scene_MainMenu::OverrideHandleInput(float dt)
{
	if (input->isKeyDown(sf::Keyboard::Escape)) {
		input->SetKeyUp(sf::Keyboard::Escape);
		window->close();
	}



}
