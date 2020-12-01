#pragma once
#include "SpriteBase.h"

class Collision
{

public:
	enum Sides//Posible contact points for a collision
	{
		Top,
		Down,
		Left,
		Right,
		None,
	};

	//AABB --> returns true if colliding
	static bool checkBoundingBox(SpriteBase* sp1, SpriteBase* sp2);


	// Bounding circle
	static bool checkBoundingSphere(SpriteBase* sp1, SpriteBase* sp2);

};

