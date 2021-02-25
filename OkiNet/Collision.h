#pragma once
#include "SpriteBase.h"

class Collision
{
public:
	// In case of a collision, where is it colliding
	struct CollisionResponse
	{
		bool None = true;

		bool s1Top = false;
		bool s1Down = false;
		bool s1Left = false;
		bool s1Right = false;

		bool s2Top = false;
		bool s2Down = false;
		bool s2Left = false;
		bool s2Right = false;
	};

	//AABB --> returns true if colliding
	static bool checkBoundingBox(SpriteBase* sp1, SpriteBase* sp2);
	// Return the collision with information of the sides
	static CollisionResponse checkBoundingBox_Sides(SpriteBase* sp1, SpriteBase* sp2);

	// Bounding circle
	static bool checkBoundingSphere(SpriteBase* sp1, SpriteBase* sp2);

};

