#include "AnimationFrame.h"

AnimationFrame::AnimationFrame()
{
	thisFrameType = AnimationFrameType::Idle;
}

AnimationFrame::AnimationFrame(sf::IntRect rect, AnimationFrameType frameType)
{
	frameRect = rect;
	thisFrameType = frameType;
}

AnimationFrame::AnimationFrame(sf::IntRect rect, AnimationFrameType frameType, CollisionBox collBox)
{
	frameRect = rect;
	thisFrameType = frameType; 
	collisionBoxes.push_back(collBox);
}

void AnimationFrame::AddCollisionBox(CollisionBox collBox)
{
	//CollisionBox* newColl = new CollisionBox(collBox);
	//collisionBoxes.push_back(newColl);
}

AnimationFrame::~AnimationFrame()
{
	//for (auto coll : collisionBoxes) delete coll;
	
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

std::vector<CollisionBox>& AnimationFrame::GetCollisionBoxes()
{
	return collisionBoxes;
}

void AnimationFrame::SetCurrentColliderPos(sf::Vector2f newPos)
{
	for (auto coll : collisionBoxes)
	{
		coll.setPosition(newPos);
	}
}
