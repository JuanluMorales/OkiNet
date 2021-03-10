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
	AnimationFrame(sf::IntRect rect, AnimationFrameType frameType, CollisionBox& collBox);
	~AnimationFrame();

	AnimationFrameType GetFrameType();
	sf::IntRect GetRect();
	void SetRect(int left, int top, int width, int height);

	CollisionBox* GetCollisionBox() { return thisCollisionBox; };
	CollisionBox::ColliderType GetColliderType() { return thisCollisionBox->GetType(); };
	bool GetColliderActive() { return thisCollisionBox->GetActive(); };
	void SetColliderDrawable(bool b) { thisCollisionBox->SetDrawable(b); };
	bool GetColliderDrawable() { return thisCollisionBox->GetDrawable(); };

private:
	AnimationFrameType thisFrameType;
	sf::IntRect frameRect;
	CollisionBox* thisCollisionBox;

};

