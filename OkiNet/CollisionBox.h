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

	CollisionBox();
	CollisionBox(ColliderType collType, sf::Color fillColor, sf::Color outlineColor, float outlineThickness, sf::Vector2f position, sf::Vector2f size);
	~CollisionBox();

	void setCollisionBox(float x, float y, float width, float height) { collRect = sf::FloatRect(x, y, width, height); };
	void setCollisionBox(sf::FloatRect fr) { collRect = fr; };
	bool isColliderActive() { return collActive; };
	void setColliderActive(bool b) { collActive = b; };

	sf::FloatRect getCollisionBox();

	ColliderType GetColliderType() { return thisCollType; };

private:
	sf::FloatRect collRect; // The rect that defines this collision box
	bool collActive; // Whether the collider is active or not
	ColliderType thisCollType; // Type of the collider for discrimination
};

