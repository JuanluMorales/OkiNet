#include "CollisionBox.h"

CollisionBox::CollisionBox()
{
	collActive = false;
	thisCollType = ColliderType::Uninitialized;
}

CollisionBox::CollisionBox(ColliderType collType, sf::Vector2f position, sf::Vector2f size)
{
	thisCollType = collType;
	SetActive(true);
	switch (collType)
	{
	case CollisionBox::ColliderType::Uninitialized:
		setFillColor(sf::Color(1, 1, 1, 100));
		setOutlineColor(sf::Color::Black);
		setOutlineThickness(1.0f);
		break;
	case CollisionBox::ColliderType::HitBox:
		setFillColor(sf::Color(255, 0, 0, 50));
		setOutlineColor(sf::Color::Red);
		setOutlineThickness(5.0f);
		break;
	case CollisionBox::ColliderType::GuardBox:
		setFillColor(sf::Color(0, 0, 255, 50));
		setOutlineColor(sf::Color::Blue);
		setOutlineThickness(5.0f);
		break;
	case CollisionBox::ColliderType::HurtBox:
		setFillColor(sf::Color(0, 255, 0, 50));
		setOutlineColor(sf::Color::Green);
		setOutlineThickness(5.0f);
		break;
	default:
		break;
	}
	SetCollisionBox(sf::FloatRect(sf::Vector2f(0, 0), size));
	setPosition(position);
	setSize(size);

}

CollisionBox::CollisionBox(ColliderType collType, sf::Color fillColor, sf::Color outlineColor, float outlineThickness, sf::Vector2f position, sf::Vector2f size)
{
	thisCollType = collType;
	SetActive(true);
	SetCollisionBox(sf::FloatRect(sf::Vector2f(0, 0), size));
	setPosition(position);
	setSize(size);
	setFillColor(fillColor);
	setOutlineColor(outlineColor);
	setOutlineThickness(outlineThickness);

}

CollisionBox::~CollisionBox()
{
}

void CollisionBox::SetType(ColliderType collType)
{
	thisCollType = collType;

	switch (collType)
	{
	case CollisionBox::ColliderType::Uninitialized:
		setFillColor(sf::Color(1, 1, 1, 100));
		setOutlineColor(sf::Color::Black);
		setOutlineThickness(1.0f);
		break;
	case CollisionBox::ColliderType::HitBox:
		setFillColor(sf::Color(255, 0, 0, 50));
		setOutlineColor(sf::Color::Red);
		setOutlineThickness(5.0f);
		break;
	case CollisionBox::ColliderType::GuardBox:
		setFillColor(sf::Color(0, 0, 255, 50));
		setOutlineColor(sf::Color::Blue);
		setOutlineThickness(5.0f);
		break;
	case CollisionBox::ColliderType::HurtBox:
		setFillColor(sf::Color(0, 255, 0, 50));
		setOutlineColor(sf::Color::Green);
		setOutlineThickness(5.0f);
		break;
	default:
		break;
	}
}

// Returns collision box + position, giving the collision box in the correct position
sf::FloatRect CollisionBox::GetCollisionBox() {

	return sf::FloatRect(collRect.left + getPosition().x, collRect.top + getPosition().y, collRect.width, collRect.height);
}
