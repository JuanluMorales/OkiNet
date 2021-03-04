#pragma once
#include "SpriteBase.h"

// Class that contains a collision box (a sprite that defines an area with collision)

class CollisionBox : public SpriteBase
{
public:
	enum class ColliderType
	{
		HitBox, // The enemy will receive damage on collision with this collider
		GuardBox, // The player does not receive damage on collision response
		HurtBox // The player receives damage on collision response
	};

	CollisionBox(ColliderType collType, sf::FloatRect collBox, sf::Color fillColor, sf::Color outlineColor, float outlineThickness, sf::Vector2f position, sf::Vector2f size);
	~CollisionBox();

	ColliderType thisCollType;

private:

};

