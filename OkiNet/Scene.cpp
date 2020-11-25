#include "Scene.h"
#include "Scene.h"

Scene::Scene(sf::RenderWindow* hwnd, InputManager* inpt, sf::View* vw)
{
	//Create window and input references
	window = hwnd;
	input = inpt;
	view = vw;

	rectShape.setSize(sf::Vector2f(50, 50));
	rectShape.setPosition(0, 0);
	rectShape.setFillColor(sf::Color::Green);

	Init();
}

Scene::~Scene() 
{
	delete shape;
}

void Scene::Init()
{
	shape = new sf::CircleShape(100.0f);
	shape->setFillColor(sf::Color::Green);
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
	HandleInput(dt);
	Update(dt);
	Render();
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

	// render something to test
	window->draw(rectShape);

	EndDraw();
}

void Scene::BeginDraw() {
	window->clear(sf::Color::Red);
}

void Scene::EndDraw() {
	window->display();
}

void Scene::CreateRectShape(sf::RectangleShape rect, float sizeX, float sizeY, float posX, float posY, sf::Color color) {
	rect.setSize(sf::Vector2f(sizeX, sizeY));
	rect.setPosition(posX, posY);
	rect.setFillColor(color);
}