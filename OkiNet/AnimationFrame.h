#pragma once
#include <SFML/Graphics.hpp>

// This animation's frames can be any of these types
enum class AnimationFrameType
{
	StartUp,
	Active,
	Recovery
};

// Class that represents an individual frame of animation and its characteristics
class AnimationFrame
{
public:
	
	AnimationFrame();
	AnimationFrame(sf::IntRect rect, AnimationFrameType frameType);
	~AnimationFrame();

	AnimationFrameType GetFrameType();
	sf::IntRect GetRect();
	void SetRect(int left, int top, int width, int height);

private:
	AnimationFrameType thisFrameType;
	sf::IntRect frameRect;


};

