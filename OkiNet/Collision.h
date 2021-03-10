#pragma once
#include "CollisionBox.h"

// Class that contains the static methods with which collision can be check between two CollisionBox, SpriteBase or Rect objects
class Collision
{
public:
	// In case of a collision, where is it colliding
	struct CollisionResponse
	{		
		bool None = true; // Variable that informs whether there was a collision or not as part of the response

		CollisionBox::ColliderType s1CollType;
		bool s1Top = false;
		bool s1Down = false;
		bool s1Left = false;
		bool s1Right = false;

		CollisionBox::ColliderType s2CollType;
		bool s2Top = false;
		bool s2Down = false;
		bool s2Left = false;
		bool s2Right = false;
	};

	//AABB --> returns true if colliding
	static bool checkBoundingBox(CollisionBox* sp1, CollisionBox* sp2);
	// Return the collision with information of the sides
	static CollisionResponse checkBoundingBox_Sides(CollisionBox* sp1, CollisionBox* sp2);

	// Bounding circle
	static bool checkBoundingSphere(SpriteBase* sp1, SpriteBase* sp2);

};

