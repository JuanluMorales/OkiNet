#pragma once
#include "SpriteBase.h"

// Class that contains a collision box (a sprite that defines an area with collision)

class CollisionBox : public SpriteBase
{
public:
	enum class ColliderType
	{
		Uninitialized,
		HitBox, // The enemy will receive damage on collision with this collider
		GuardBox, // The player does not receive damage on collision response
		HurtBox // The player receives damage on collision response
	};

	CollisionBox();
	CollisionBox(ColliderType collType, sf::Color fillColor, sf::Color outlineColor, float outlineThickness, sf::Vector2f position, sf::Vector2f size);
	~CollisionBox();

	inline void SetCollisionBox(float x, float y, float width, float height) { collRect = sf::FloatRect(x, y, width, height); };
	inline void SetCollisionBox(sf::FloatRect fr) { collRect = fr; };
	inline bool IsActive() { return collActive; };
	inline void SetActive(bool b) { collActive = b; };
	inline void SetToBeDrawn(bool b) { SpriteBase::SetActive(b); }; // Set whether the collision box should be drawn or not
	bool ShouldBeDrawn() { return SpriteBase::IsActive(); }; // Returns whether the sprite is active or not
	sf::FloatRect GetCollisionBox();

	ColliderType GetType() const { return thisCollType; };

private:
	sf::FloatRect collRect; // The rect that defines this collision box
	bool collActive; // Whether the collider is active or not for collision check purposes
	ColliderType thisCollType; // Type of the collider for discrimination
};

