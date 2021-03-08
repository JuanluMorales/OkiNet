#include "SpriteBase.h"

SpriteBase::SpriteBase()
{

}

SpriteBase::~SpriteBase()
{}

// override this function to provide functionality
void SpriteBase::update(float dt)
{
}

// Sets the velocity of the sprite
void SpriteBase::setVelocity(sf::Vector2f vel)
{
	velocity = vel;
}
void SpriteBase::setVelocity(float vx, float vy)
{
	velocity.x = vx;
	velocity.y = vy;
}

// get sprite velocity
sf::Vector2f SpriteBase::getVelocity()
{
	return velocity;
}
