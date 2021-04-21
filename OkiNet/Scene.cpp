#include "Scene.h"
#include "Scene.h"

Scene::Scene(sf::RenderWindow* hwnd, InputManager* inpt, sf::View* vw)
{
	//Create window and input references
	window = hwnd;
	input = inpt;
	view = vw;
	clearColor = sf::Color::White;

	 Init();
}

Scene::~Scene() 
{
}

void Scene::Init()
{
}

void Scene::OverrideEarlyUpdate(float dt)
{
}

void Scene::OverrideRender()
{
}

void Scene::OverrideUpdate(float dt)
{
}

void Scene::OverrideHandleInput(float dt)
{
}

void Scene::RunScene(float dt)
{
	if(ShouldHandleInput) HandleInput(dt);
	if(ShouldUpdate) Update(dt);
	if(ShouldRender) Render();
}

void Scene::HandleInput(float dt)
{
	OverrideHandleInput(dt);
}

void Scene::Update(float dt)
{
	//update view
	window->setView(*view);

	// Call virtual function to add functionality from inherited classes
	OverrideUpdate(dt);
}

void Scene::Render()
{
	BeginDraw();

	// Call inherited virtual function to include render objects
	OverrideRender();

	EndDraw();
}

void Scene::BeginDraw() {
	window->clear(clearColor);
}

void Scene::EndDraw() {
	window->display();
}

void Scene::CreateRectShape(sf::RectangleShape rect, float sizeX, float sizeY, float posX, float posY, sf::Color color) {
	rect.setSize(sf::Vector2f(sizeX, sizeY));
	rect.setPosition(posX, posY);
	rect.setFillColor(color);
}