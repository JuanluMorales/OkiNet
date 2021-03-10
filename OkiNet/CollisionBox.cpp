#include "CollisionBox.h"

CollisionBox::CollisionBox()
{
	collActive = false;
	thisCollType = ColliderType::Uninitialized;
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

// Returns collision box + position, giving the collision box in the correct position
sf::FloatRect CollisionBox::GetCollisionBox() {

	return sf::FloatRect(collRect.left + getPosition().x, collRect.top + getPosition().y, collRect.width, collRect.height);
}
