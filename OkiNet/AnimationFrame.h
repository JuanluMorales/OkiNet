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

// Class that represents an individual frame of animation and its characteristics such as its type and (optional) collision box
class AnimationFrame
{
public:
	AnimationFrame();
	AnimationFrame(sf::IntRect rect, AnimationFrameType frameType);
	AnimationFrame(sf::IntRect rect, AnimationFrameType frameType, CollisionBox collBox);
	void AddCollisionBox(CollisionBox collBox);
	~AnimationFrame();

	AnimationFrameType GetFrameType();
	sf::IntRect GetRect();
	void SetRect(int left, int top, int width, int height);

	std::vector<CollisionBox>& GetCollisionBoxes(); // Returns the list of collision boxes in this frame
	void SetCurrentColliderPos(sf::Vector2f newPos);

private:
	AnimationFrameType thisFrameType;
	sf::IntRect frameRect;
	std::vector<CollisionBox> collisionBoxes; // A frame may have more than one box of different types

};

