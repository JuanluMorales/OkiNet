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
		HurtBox, // The player receives damage on collision response
		Environment // The collider blocks the player movement
	};

	CollisionBox();
	CollisionBox(ColliderType collType, sf::Vector2f position, sf::Vector2f size, sf::Vector2f posOffset = sf::Vector2f(0,0)); // Will automatically assign fill and outline color and thickness based on type
	CollisionBox(ColliderType collType, sf::Color fillColor, sf::Color outlineColor, float outlineThickness, sf::Vector2f position, sf::Vector2f size, sf::Vector2f posOffset = sf::Vector2f(0,0));
	~CollisionBox();

	void SetCollisionBox(float x, float y, float width, float height) { collRect = sf::FloatRect(x, y, width, height); };
	void SetCollisionBox(sf::FloatRect fr) { collRect = fr; };

	sf::FloatRect GetCollisionBox(); // Returns the rect properties of the collider
	ColliderType GetType() { return thisCollType; };
	// Change the collision box to the new collision type along with its color properties
	void SetType(ColliderType collType);

	// Is the collider active for collision registering purposes?
	bool GetActive() { return collActive; };
	void SetActive(bool b) { collActive = b; };

	void SetDrawable(bool b) { SpriteBase::SetActive(b); }; // Set whether the collision box should be drawn or not
	bool GetDrawable() { return SpriteBase::IsActive(); }; // Returns whether the sprite is active or not

	// An offset must be specified depending on the needs of the collision box positioning, add it to the position update to keep the order
	void SetPositionOffset(sf::Vector2f v2f) { offsetValue = v2f; };
	sf::Vector2f GetPositionOffset() { return offsetValue; };

	// Use this method to account for the offset registered
	void SetCollisionBoxPosition(sf::Vector2f newPos);

private:
	sf::FloatRect collRect; // The rect that defines this collision box
	bool collActive; // Whether the collider is active or not for collision check purposes
	ColliderType thisCollType; // Type of the collider for discrimination
	sf::Vector2f offsetValue; // A value added to the frame to be positioned
};

