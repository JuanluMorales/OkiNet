#include "Collision.h"

// check AABB
bool Collision::checkBoundingBox(SpriteBase* s1, SpriteBase* s2)
{
	if (s1->getCollisionBox().left + s1->getCollisionBox().width < s2->getCollisionBox().left)
		return false;

	if (s1->getCollisionBox().left > s2->getCollisionBox().left + s2->getCollisionBox().width)
		return false;

	if (s1->getCollisionBox().top + s1->getCollisionBox().height < s2->getCollisionBox().top)
		return false;

	if (s1->getCollisionBox().top > s2->getCollisionBox().top + s2->getCollisionBox().height)
		return false;

	return true;
}

Collision::CollisionResponse Collision::checkBoundingBox_Sides(SpriteBase* s1, SpriteBase* s2)
{
	sf::FloatRect s1FR = s1->getCollisionBox();
	sf::FloatRect s2FR = s2->getCollisionBox();

	CollisionResponse newResponse;

	if (s1->getCollisionBox().left + s1->getCollisionBox().width < s2->getCollisionBox().left)
		return newResponse;

	if (s1->getCollisionBox().left > s2->getCollisionBox().left + s2->getCollisionBox().width)
		return newResponse;

	if (s1->getCollisionBox().top + s1->getCollisionBox().height < s2->getCollisionBox().top)
		return newResponse;

	if (s1->getCollisionBox().top > s2->getCollisionBox().top + s2->getCollisionBox().height)
		return newResponse;

	// If checks passed, a collision must have occurred
	newResponse.None = false;

	// s1R > s2L
	if (s1->getCollisionBox().left + s1->getCollisionBox().width > s2->getCollisionBox().left)
	{
		newResponse.s1Right = true;
		newResponse.s2Left = true;
	}

	// s1L < s2R
	if (s1->getCollisionBox().left < s2->getCollisionBox().left + s2->getCollisionBox().width)
	{
		newResponse.s1Left = true;
		newResponse.s2Right = true;
	}

	// s1u > s2D
	if (s1->getCollisionBox().top + s1->getCollisionBox().height > s2->getCollisionBox().top)
	{
		newResponse.s1Top = true;
		newResponse.s2Down = true;
	}

	// s1D < s2U
	if (s1->getCollisionBox().top < s2->getCollisionBox().top + s2->getCollisionBox().height)
	{
		newResponse.s1Down = true;
		newResponse.s2Top = true;
	}

	return newResponse;
}


// check Sphere bounding collision
bool Collision::checkBoundingSphere(SpriteBase* s1, SpriteBase* s2)
{
	// Get radius and centre of sprites.
	float radius1 = s1->getSize().x / 2;
	float radius2 = s2->getSize().x / 2;
	float xpos1 = s1->getPosition().x + radius1;
	float xpos2 = s2->getPosition().x + radius2;
	float ypos1 = s1->getPosition().y + radius1;
	float ypos2 = s2->getPosition().y + radius2;

	if (pow(xpos2 - xpos1, 2) + pow(ypos2 - ypos1, 2) < pow(radius1 + radius2, 2))
	{
		return true;
	}
	return false;
}