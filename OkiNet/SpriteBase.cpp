#include "SpriteBase.h"

SpriteBase::SpriteBase()
{}

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

// Returns collision box + position, giving the collision box in the correct position
sf::FloatRect SpriteBase::getCollisionBox() {

	return sf::FloatRect(collisionBox.left + getPosition().x, collisionBox.top + getPosition().y, collisionBox.width, collisionBox.height);
}

// Reponse function, what the sprite does based on collision
// Colliding object is passed in for information
// e.g. compare sprite positions to determine new velocity direction.
// e.g. checking sprite type (world, enemy, bullet etc) so response is based on that.
void SpriteBase::collisionResponse(SpriteBase* sp)
{

}
