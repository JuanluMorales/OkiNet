#pragma once
#include <vector>
#include "AnimationFrame.h"

// Contains an ensemble of frames to convey animation
class Animation
{
public:
	Animation();

	// Create animation
	void AddFrame(AnimationFrame animFrame); // Not stable/tested
	void AddFrame(sf::IntRect rect, AnimationFrameType animType); // Add an animation without collision
	void AddFrame(sf::IntRect rect, AnimationFrameType animType, CollisionBox collBox); // Add animation with a single collision box
	void AddFrame(sf::IntRect rect, AnimationFrameType animType, std::vector<CollisionBox*>& collBoxes); // Add animation with composite collision
	void SetFrameSpeed(float speed);
	int GetSize();

	void SetID(int id) { ID = id; }
	int GetID() { return ID; }

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
	void SetFlipped(bool flip);
	bool IsAimationFlipped() { return flipped; };
	void SetLooping(bool loop) { isLooping = loop; };
	bool IsAnimationLooping() { return isLooping; };
	void SetPlaying(bool play) { isPlaying = play; };
	bool IsAnimationPlaying() { return isPlaying; };
	int GetAnimationFramesCount(); //returns the number of frames this animation has

	// Collision box controls
	std::vector<CollisionBox*> GetCurrentBoxes(); // return a vector of the current collision boxes for the frame

protected:
	std::vector<AnimationFrame>frames; // The total of frames in the animation
	AnimationFrame frame;
	int currentFrame;
	float animationSpeed;	// in FPS
	bool flipped;
	bool isLooping;
	bool isPlaying;
	float elapsedTime;
	int ID; // Animation identifier

};

