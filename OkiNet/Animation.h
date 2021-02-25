#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

// Contains an ensemble of sprites to convey animation
class Animation
{
public:
	Animation();

	// Create animation
	void addFrame(sf::IntRect rect);
	void setFrameSpeed(float speed);
	int getSize();

	// update/reset animation
	void animate(float dt);
	void reset();

	// get current frame of animation
	sf::IntRect getCurrentFrame();
	// Returns whether the animation finished looping through all its frames
	bool isAnimationCompleted();
	// Returns whether or not the animation has reached N frame, useful for checking if we are over the active frames and we are in recover frames
	bool hasAnimationReachedFrame(int frameNumber); 

	// Animation controls
	float getFrameSpeed() { return animationSpeed; };
	void setFlipped(bool flip) { flipped = flip; };
	bool getFlipped() { return flipped; };
	void setLooping(bool loop) { isLooping = loop; };
	bool getLooping() { return isLooping; };
	void setPlaying(bool play) { isPlaying = play; };
	bool getPlaying() { return isPlaying; };
	int GetAnimationFramesCount(); //returns the number of frames this animation has

protected:
	std::vector<sf::IntRect>frames;
	sf::IntRect frame;
	int currentFrame;
	float animationSpeed;	// in FPS
	bool flipped;
	bool isLooping;
	bool isPlaying;
	float elapsedTime;

};

