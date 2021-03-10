#pragma once
#include <SFML/Graphics.hpp>

class SpriteBase : public sf::RectangleShape
{
public:
	SpriteBase();
	~SpriteBase();

	virtual void Update(float dt);

	// sprite speed and direction
	void SetVelocity(sf::Vector2f vel);
	void SetVelocity(float vx, float vy);
	sf::Vector2f GetVelocity();

	// sprite state
	bool IsActive() { return active; };
	void SetActive(bool b) { active = b; };

	enum class SpriteType //Helps discriminate this sprite
	{
		Uninitialized,
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
	bool active;
	bool flipped = false; // Is the sprite flipped
};

