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