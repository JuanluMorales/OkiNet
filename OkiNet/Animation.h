#pragma once
#include <vector>
#include "AnimationFrame.h"

// Contains an ensemble of sprites to convey animation
class Animation
{
public:
	Animation();

	// Create animation
	void AddFrame(sf::IntRect rect, AnimationFrameType animType);
	void SetFrameSpeed(float speed);
	int GetSize();

	// update/reset animation
	void Animate(float dt);
	void ResetAnimation();

	// get current frame of animation
	AnimationFrame GetCurrentFrame();
	// Returns whether the animation finished looping through all its frames
	bool IsAnimationCompleted();
	// Returns whether or not the animation has reached N frame, useful for checking if we are over the active frames and we are in recover frames
	bool HasAnimationReachedFrame(int frameNumber); 

	// Animation controls
	float GetFrameSpeed() { return animationSpeed; };
	void SetFlipped(bool flip) { flipped = flip; };
	bool IsAimationFlipped() { return flipped; };
	void SetLooping(bool loop) { isLooping = loop; };
	bool IsAnimationLooping() { return isLooping; };
	void SetPlaying(bool play) { isPlaying = play; };
	bool IsAnimationPlaying() { return isPlaying; };
	int GetAnimationFramesCount(); //returns the number of frames this animation has

protected:
	std::vector<AnimationFrame>frames;
	AnimationFrame frame;
	int currentFrame;
	float animationSpeed;	// in FPS
	bool flipped;
	bool isLooping;
	bool isPlaying;
	float elapsedTime;

};

