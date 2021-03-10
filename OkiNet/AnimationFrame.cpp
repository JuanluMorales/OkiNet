#include "AnimationFrame.h"

AnimationFrame::AnimationFrame()
{
	thisFrameType = AnimationFrameType::Idle;
	collisionBox = new CollisionBox();
}

AnimationFrame::AnimationFrame(sf::IntRect rect, AnimationFrameType frameType)
{
	frameRect = rect;
	thisFrameType = frameType;
	collisionBox = new CollisionBox();
}

AnimationFrame::AnimationFrame(sf::IntRect rect, AnimationFrameType frameType, CollisionBox collBox)
{
	frameRect = rect;
	thisFrameType = frameType; 
	collisionBox = new CollisionBox(collBox.GetType(), collBox.getPosition(), collBox.getSize());
}

void AnimationFrame::AddCollisionBox(CollisionBox& collBox)
{
	collisionBox = new CollisionBox(collBox);
}

AnimationFrame::~AnimationFrame()
{	
}

AnimationFrameType AnimationFrame::GetFrameType()
{
	return thisFrameType;
}

sf::IntRect AnimationFrame::GetRect()
{
	return frameRect;
}

void AnimationFrame::SetRect(int left, int top, int width, int height)
{
	frameRect.left = left;
	frameRect.top = top;
	frameRect.width = width;
	frameRect.height = height;
}

CollisionBox* AnimationFrame::GetCollisionBox()
{
	return collisionBox;
}

void AnimationFrame::SetCurrentColliderPos(sf::Vector2f newPos)
{
	collisionBox->setPosition(newPos);
}
