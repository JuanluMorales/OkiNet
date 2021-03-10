#include "Animation.h"

// Constructor. Sets initial values.
Animation::Animation()
{
	flipped = false;
	currentFrame = 0;
	elapsedTime = 0.f;
	isPlaying = true;
	isLooping = true;
	animationSpeed = 1;

}

// Adds animation frame. Rect object represent a single sprite frame
void Animation::AddFrame(sf::IntRect rect, AnimationFrameType animType)
{
	AnimationFrame* anim = new AnimationFrame(rect, animType);
	frames.push_back(*anim);
}

//void Animation::AddFrame(sf::IntRect rect, AnimationFrameType animType, CollisionBox* collBox)
//{
//	AnimationFrame* anim = new AnimationFrame(rect, animType, *collBox);
//	frames.push_back(*anim);
//	boxes.push_back(collBox);
//}

// Returns the size of the animation. The number of frames.
int Animation::GetSize()
{
	return static_cast<int>(frames.size());
}

// Get current frame from the animation. Flip the returning frame if required.
AnimationFrame Animation::GetCurrentFrame()
{
	frame = frames[currentFrame];
	if (flipped)
	{
		sf::IntRect frameRect = frame.GetRect();
		frame.SetRect(frameRect.left + frameRect.width, frameRect.top, -frameRect.width, frameRect.height);
	}
	return frame;
}

bool Animation::IsAnimationCompleted()
{
	if (currentFrame == frames.size() - 1)
	{
		return true;
	}
	else return false;


}

bool Animation::HasAnimationReachedFrame(int frameNumber)
{
	if (frameNumber == currentFrame)
	{
		return true;
	}
	else return false;
}

// Check if the animation should progress, if so move to next frame, or loop back to the start
void Animation::Animate(float dt)
{
	if (isPlaying)
	{
		elapsedTime += dt;
		if (elapsedTime >= animationSpeed)
		{
			currentFrame++;
			if (currentFrame >= (int)frames.size())
			{
				if (isLooping)
				{
					currentFrame = 0;
				}
				else
				{
					currentFrame--;
				}

			}
			elapsedTime = 0;
		}
	}
}

// Reset animation.
void Animation::ResetAnimation()
{
	currentFrame = 0;
	elapsedTime = 0;
}

// Set animation speed, in Frames per Second.
void Animation::SetFrameSpeed(float speed)
{
	animationSpeed = speed;
}

//returns the number of frames this animation has
int Animation::GetAnimationFramesCount() {
	return static_cast<int>(frames.size());
}
