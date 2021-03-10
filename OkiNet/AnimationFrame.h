#pragma once
#include <SFML/Graphics.hpp>
#include "Collision.h"

// This animation's frame type 
enum class AnimationFrameType
{
	Idle, // A looping animation 
	StartUp, // The attack is winding up
	Active, // The attack can hurt if the hitbox connects
	Recovery // The player makes a follow through 
};

// Class that represents an individual frame of animation and its characteristics
class AnimationFrame
{
public:
	AnimationFrame();
	AnimationFrame(sf::IntRect rect, AnimationFrameType frameType);
	AnimationFrame(sf::IntRect rect, AnimationFrameType frameType, CollisionBox& collBox);
	~AnimationFrame();

	AnimationFrameType GetFrameType();
	sf::IntRect GetRect();
	void SetRect(int left, int top, int width, int height);

private:
	AnimationFrameType thisFrameType;
	sf::IntRect frameRect;
	CollisionBox* thisCollisionBox;

};

