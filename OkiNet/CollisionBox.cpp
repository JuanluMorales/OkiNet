#include "CollisionBox.h"

CollisionBox::CollisionBox()
{
	collActive = false;
	thisCollType = ColliderType::Uninitialized;
	thisSpriteType = SpriteType::Collider;
	SetDrawable(false);
	SetActive(false);
	setFillColor(sf::Color(0, 0, 0, 100));
	setOutlineColor(sf::Color::Black);
	setOutlineThickness(1.0f);
}

CollisionBox::CollisionBox(ColliderType collType, sf::Vector2f position, sf::Vector2f size, sf::Vector2f posOffset)
{
	thisCollType = collType;
	thisSpriteType = SpriteType::Collider;
	SetPositionOffset(posOffset);
	SetActive(true);
	switch (collType)
	{
	case CollisionBox::ColliderType::Uninitialized:
		setFillColor(sf::Color(0, 0, 0, 100));
		setOutlineColor(sf::Color::Black);
		setOutlineThickness(1.0f);
		break;
	case CollisionBox::ColliderType::HitBox:
		setFillColor(sf::Color(255, 0, 0, 50));
		setOutlineColor(sf::Color::Red);
		setOutlineThickness(5.0f);
		break;
	case CollisionBox::ColliderType::GuardBox:
		setFillColor(sf::Color(0, 0, 255, 50));
		setOutlineColor(sf::Color::Blue);
		setOutlineThickness(5.0f);
		break;
	case CollisionBox::ColliderType::HurtBox:
		setFillColor(sf::Color(0, 255, 0, 50));
		setOutlineColor(sf::Color::Green);
		setOutlineThickness(5.0f);
		break;
	default:
		break;
	}
	SetCollisionBox(sf::FloatRect(sf::Vector2f(0, 0), size));

	setSize(size);
	setOrigin(sf::Vector2f(size.x / 2, size.y / 2));
	setPosition(position);
}

CollisionBox::CollisionBox(ColliderType collType, sf::Color fillColor, sf::Color outlineColor, float outlineThickness, sf::Vector2f position, sf::Vector2f size, sf::Vector2f posOffset)
{
	thisCollType = collType;
	SetPositionOffset(posOffset);
	thisSpriteType = SpriteType::Collider;
	SetActive(true);
	SetCollisionBox(sf::FloatRect(sf::Vector2f(0, 0), size));
	setOrigin(sf::Vector2f(size.x / 2, size.y / 2));
	setSize(size);
	setPosition(position);

	setFillColor(fillColor);
	setOutlineColor(outlineColor);
	setOutlineThickness(outlineThickness);

}

CollisionBox::~CollisionBox()
{
}

void CollisionBox::SetType(ColliderType collType)
{
	thisCollType = collType;

	switch (collType)
	{
	case CollisionBox::ColliderType::Uninitialized:
		setFillColor(sf::Color(1, 1, 1, 100));
		setOutlineColor(sf::Color::Black);
		setOutlineThickness(1.0f);
		break;
	case CollisionBox::ColliderType::HitBox:
		setFillColor(sf::Color(255, 0, 0, 50));
		setOutlineColor(sf::Color::Red);
		setOutlineThickness(5.0f);
		break;
	case CollisionBox::ColliderType::GuardBox:
		setFillColor(sf::Color(0, 0, 255, 50));
		setOutlineColor(sf::Color::Blue);
		setOutlineThickness(5.0f);
		break;
	case CollisionBox::ColliderType::HurtBox:
		setFillColor(sf::Color(0, 255, 0, 50));
		setOutlineColor(sf::Color::Green);
		setOutlineThickness(5.0f);
		break;
	default:
		break;
	}
}

void CollisionBox::SetCollisionBoxPosition(sf::Vector2f newPos)
{
	if (flipped && GetType() == ColliderType::HitBox)
	{
		setPosition(newPos + sf::Vector2f(-offsetValue.x, offsetValue.y));
	}else setPosition(newPos + offsetValue);
}

// Returns collision box + position, giving the collision box in the correct position
sf::FloatRect CollisionBox::GetCollisionBox() {

	return sf::FloatRect(collRect.left + getPosition().x , collRect.top + getPosition().y, collRect.width, collRect.height);
}
