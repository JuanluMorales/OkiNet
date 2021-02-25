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
void Animation::addFrame(sf::IntRect rect)
{
	frames.push_back(rect);
}

// Returns the size of the animation. The number of frames.
int Animation::getSize()
{
	return frames.size();
}

// Get current frame from the animation. Flip the returning frame if required.
sf::IntRect Animation::getCurrentFrame()
{
	frame = frames[currentFrame];
	if (flipped)
	{

		frame = sf::IntRect(frame.left + frame.width, frame.top, -frame.width, frame.height);
	}
	return frame;
}

bool Animation::isAnimationCompleted()
{
	// Find the current frame
	//auto it = std::find(frames.begin(), frames.end(), frames[currentFrame]);

	//if (it != frames.end()) // If element was found
	//{
	//	int index = it - frames.begin();

	//	if (index == frames.size() - 1)
	//	{
	//		return true;
	//	}
	//	else return false;
	//}
	//else {
	//	// Element could not be found
	//	return false;
	//}

	if (currentFrame == frames.size() - 1)
	{
		return true;
	}
	else return false;


}

// Check if the animation should progress, if so move to next frame, or loop back to the start
void Animation::animate(float dt)
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
void Animation::reset()
{
	currentFrame = 0;
	elapsedTime = 0;
}

// Set animation speed, in Frames per Second.
void Animation::setFrameSpeed(float speed)
{
	animationSpeed = speed;
}

//returns the number of frames this animation has
int Animation::GetAnimationFramesCount() {
	return frames.size();
}