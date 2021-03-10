#include "SpriteBase.h"

SpriteBase::SpriteBase()
{
	active = true;
	thisSpriteType = SpriteType::Uninitialized;
}

SpriteBase::~SpriteBase()
{}

// override this function to provide functionality
void SpriteBase::Update(float dt)
{
}

// Sets the velocity of the sprite
void SpriteBase::SetVelocity(sf::Vector2f vel)
{
	velocity = vel;
}
void SpriteBase::SetVelocity(float vx, float vy)
{
	velocity.x = vx;
	velocity.y = vy;
}

// get sprite velocity
sf::Vector2f SpriteBase::GetVelocity()
{
	return velocity;
}
