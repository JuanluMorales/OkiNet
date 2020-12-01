#pragma once
#include <SFML/Graphics.hpp>

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

	// for sprite collision
	bool isCollider() { return collider; };
	void setCollider(bool b) { collider = b; };
	sf::FloatRect getCollisionBox();
	void setCollisionBox(float x, float y, float width, float height) { collisionBox = sf::FloatRect(x, y, width, height); };
	void setCollisionBox(sf::FloatRect fr) { collisionBox = fr; };
	virtual void collisionResponse(SpriteBase* sp);

	//Animations that can be created through AddNewAnimation() for static object such as tiles from the tilemap
	//std::vector<Animation> animations;

	enum class SpriteType //Helps discriminate this sprite
	{
		Player,
		Enemy
	};

	SpriteType thisSpriteType;

	const int PIXEL_SCALE_FACTOR = 3; //Scale factor applied to the sprites to upscale them
	const double PI = 3.14159265; //just pi

protected:
	// Sprite properties
	sf::Vector2f velocity;
	bool alive;

	// Collision vars
	sf::FloatRect collisionBox;
	bool collider;
};

