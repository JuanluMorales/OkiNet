#pragma once
#include <SFML/Graphics.hpp>
#include "Animation.h"

class SpriteBase : public sf::RectangleShape
{
public:
	SpriteBase();
	~SpriteBase();

	virtual void update(float dt);

	// sprite speed and direction
	void setVelocity(sf::Vector2f vel);
	void setVelocity(float vx, float vy);
	sf::Vector2f getVelocity();

	// sprite state
	bool isAlive() { return alive; };
	void setAlive(bool b) { alive = b; };

	enum class SpriteType //Helps discriminate this sprite
	{
		Player,
		Enemy,
		Environment,
		Collider
	};

	SpriteType thisSpriteType;

	const int PIXEL_SCALE_FACTOR = 10; //Scale factor applied to the sprites to upscale them
	const double PI = 3.14159265; //just pi

protected:
	// Sprite properties
	sf::Vector2f velocity;
	bool alive;
	bool flipped = false; // Is the sprite flipped
};

