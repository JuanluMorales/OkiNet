#include "CollisionBox.h"

CollisionBox::CollisionBox(ColliderType collType, sf::FloatRect collBox, sf::Color fillColor, sf::Color outlineColor, float outlineThickness, sf::Vector2f position, sf::Vector2f size)
{
	thisCollType = collType;
	setCollider(true);
	setCollisionBox(collBox);
	setPosition(position);
	setSize(size);
	setFillColor(fillColor);
	setOutlineColor(outlineColor);
	setOutlineThickness(outlineThickness);

}

CollisionBox::~CollisionBox()
{
}
