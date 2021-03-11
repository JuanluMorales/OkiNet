#include "AnimationFrame.h"

AnimationFrame::AnimationFrame()
{
	thisFrameType = AnimationFrameType::Idle;
	collisionBox = new CollisionBox();
	collisionBoxes.push_back(collisionBox);
}

AnimationFrame::AnimationFrame(sf::IntRect rect, AnimationFrameType frameType)
{
	frameRect = rect;
	thisFrameType = frameType;
	collisionBox = new CollisionBox();
	collisionBoxes.push_back(collisionBox);
}

AnimationFrame::AnimationFrame(sf::IntRect rect, AnimationFrameType frameType, CollisionBox collBox)
{
	frameRect = rect;
	thisFrameType = frameType; 
	collisionBox = new CollisionBox(collBox.GetType(), collBox.getPosition(), collBox.getSize());
	collisionBoxes.push_back(collisionBox);
}

AnimationFrame::AnimationFrame(sf::IntRect rect, AnimationFrameType frameType, std::vector<CollisionBox*>& collBox)
{
	frameRect = rect;
	thisFrameType = frameType;
	collisionBoxes = collBox;
}

void AnimationFrame::AddCollisionBox(CollisionBox& collBox)
{
	collisionBox = new CollisionBox(collBox);
	collisionBoxes.push_back(collisionBox);
}

AnimationFrame::~AnimationFrame()
{	
	for (auto coll : collisionBoxes)
	{
		coll = NULL;
		delete coll;
	}
	//collisionBox = NULL;
	//delete collisionBox;
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

std::vector<CollisionBox*> AnimationFrame::GetCollisionBoxes()
{
	return collisionBoxes;
}
