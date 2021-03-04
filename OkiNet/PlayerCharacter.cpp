#include "PlayerCharacter.h"

PlayerCharacter::PlayerCharacter()
{
	// Set character defaults
	playerID = PlayerID::PlayerOne;
	playerState = PlayerState::Alive; // Alive by default
	moveState = MoveState::Idle; // Idle by default

	grounded = false; //Sets the player able to jump when true
	CanGoLeft = true; //Allows the player to move left 
	CanGoRight = true;
	CanGoUp = true; //If theres a ceiling, doesnt allow to go up
	maxHealthPoints = 50; //Total hit points the player can suffer before dying
	currentHealthPoints = 50; //Current health 
	MoveSpeed = 100; //Multiplier for the movement speed
	currentAnim = NULL;
	CharacterSetUp = false;
}

/// <summary>
/// Initialize the player character. Must be called at scene initialization to setup different players
/// </summary>
/// <param name="id"></param> Whether this is player one or two
/// <param name="startPos"></param> Spawn location in the world
void PlayerCharacter::InitCharacter(PlayerID id, sf::Vector2f startPos)
{
	playerID = id;

	if (id == PlayerID::PlayerOne)
	{
		// Configure player one graphics
		texture.loadFromFile("Sprites/Player1_Sheet.png");
		setSize(sf::Vector2f(78, 55));

		setScale( PIXEL_SCALE_FACTOR, PIXEL_SCALE_FACTOR);
		setTexture(&texture);
	}
	else
	{
		// Configure player two graphics
		texture.loadFromFile("Sprites/Player2_Sheet.png");
		setSize(sf::Vector2f(78, 55));
		setScale(PIXEL_SCALE_FACTOR, PIXEL_SCALE_FACTOR);
		setTexture(&texture);
		flipped = true;
	}

	// Setup Animations
	SetUpAnimations();

	setPosition(startPos);

	// Setup collision
	// Body collision
	bodyCollOffset = sf::Vector2f(25 * PIXEL_SCALE_FACTOR, 5 * PIXEL_SCALE_FACTOR);
	if (flipped) bodyCollOffset += sf::Vector2f(15, 0); // Theres an issue when flipping the colliders that will offset them for an amount, correct it adding this to the flip colliders
	bodyColl.setCollider(true);
	sf::Vector2f bodycallPos = getPosition() + bodyCollOffset;
	sf::Vector2f bodycallSize = sf::Vector2f(25 * PIXEL_SCALE_FACTOR, 42 * PIXEL_SCALE_FACTOR);
	bodyColl.setCollisionBox(sf::FloatRect(sf::Vector2f(0, 0), bodycallSize));
	bodyColl.setOutlineColor(sf::Color::Green);
	bodyColl.setOutlineThickness(5.f);
	bodyColl.setFillColor(sf::Color::Transparent);
	bodyColl.setPosition(bodycallPos);
	bodyColl.setSize(bodycallSize);



	CharacterSetUp = true;
}

PlayerCharacter::~PlayerCharacter()
{
}

//Manages the movement and animation of the player
void PlayerCharacter::update(float dt, sf::Window* wnd)
{
	// Update collider
	sf::Vector2f newPos = getPosition() + bodyCollOffset;
	bodyColl.setPosition(newPos);

	HandleAnimation(dt);

	switch (animState)
	{
	case AnimationFrameType::Idle:
		shouldAcceptInput = true;
		break;
	case AnimationFrameType::StartUp:
		shouldAcceptInput = false;
		break;
	case AnimationFrameType::Active:
		shouldAcceptInput = false;
		break;
	case AnimationFrameType::Recovery:
		shouldAcceptInput = true;
		break;
	default:
		break;
	}

}

void PlayerCharacter::handleInput(InputManager* input, float dt)
{
	if (playerID == PlayerID::PlayerOne)
	{
		if (!input->isKeyDown(sf::Keyboard::S) && currentAnim == &anim_defend)
		{
			shouldAcceptInput = true;
			attackState = AttackState::None;
		}

		if (!shouldAcceptInput) return;

		if (input->isKeyDown(sf::Keyboard::S))
		{
			attackState = AttackState::Defend;
			animState = AnimationFrameType::StartUp;
		} 

		if (input->isKeyDown(sf::Keyboard::Q))
		{
			attackState = AttackState::FastPunch;
			animState = AnimationFrameType::StartUp;
		}

		// Placeholder movement
		if (input->isKeyDown(sf::Keyboard::A))
		{
			setPosition(getPosition().x - MoveSpeed * dt, getPosition().y);
			moveState = MoveState::Left;
		}
		else
			if (input->isKeyDown(sf::Keyboard::D))
			{
				setPosition(getPosition().x + MoveSpeed * dt, getPosition().y);
				moveState = MoveState::Right;
			}
			else moveState = MoveState::Idle;
	}
	else if (playerID == PlayerID::PlayerTwo)
	{

		if (!shouldAcceptInput) return;

		if (input->isKeyDown(sf::Keyboard::Left))
		{
			setPosition(getPosition().x - MoveSpeed * dt, getPosition().y);
			moveState = MoveState::Left;
		}
		else
			if (input->isKeyDown(sf::Keyboard::Right))
			{
				setPosition(getPosition().x + MoveSpeed * dt, getPosition().y);
				moveState = MoveState::Right;
			}
			else moveState = MoveState::Idle;
	}


}

void PlayerCharacter::HandleAnimation(float dt)
{
	bool isAttacking = false; // track if the character is attacking this frame

	switch (attackState)
	{
	case AttackState::None:
		break;
	case AttackState::FastPunch:
		if (currentAnim->isAnimationCompleted() && animState == AnimationFrameType::StartUp) // TODO: modify condition so that it checks if this is the loop where we inputted the action
		{
			anim_fastPunch.reset();
			currentAnim = &anim_fastPunch;
		}else
		if (currentAnim->isAnimationCompleted())
		{
			anim_fastPunch.reset();
			attackState = AttackState::None;
		}
		else
		{
			currentAnim = &anim_fastPunch;
		}

		break;
	case AttackState::Defend:
		currentAnim = &anim_defend;
		break;
	default:
		break;
	}

	if (attackState != AttackState::None) isAttacking = true;

	if (!isAttacking)
	{
		// Update movement animation
		switch (moveState)
		{
		case MoveState::Idle:
			currentAnim = &anim_idle;
			break;
		case MoveState::Right:
			currentAnim = &anim_walkFWD;
			break;
		case MoveState::Left:
			currentAnim = &anim_walkFWD;
			break;
		default:
			break;
		}
	}

	// Remember the animation class of the flip state of the sprite
	if (flipped)
	{
		currentAnim->setFlipped(true);
	}


	currentAnim->animate(dt);
	setTextureRect(currentAnim->getCurrentFrame().GetRect());
	setFillColor(sf::Color(getFillColor().r, getFillColor().g, getFillColor().b, 250));

	// Set the animation state (startup, active, recovery...)
	animState = currentAnim->getCurrentFrame().GetFrameType();
}

void PlayerCharacter::SetUpAnimations()
{
	anim_idle.addFrame(sf::IntRect(0, 0, 78, 55), AnimationFrameType::Idle);
	anim_idle.addFrame(sf::IntRect(78, 0, 78, 55), AnimationFrameType::Idle);
	anim_idle.addFrame(sf::IntRect(156, 0, 78, 55), AnimationFrameType::Idle);
	anim_idle.addFrame(sf::IntRect(234, 0, 78, 55), AnimationFrameType::Idle);
	anim_idle.setFrameSpeed(0.15f);

	anim_walkFWD.addFrame(sf::IntRect(0, 55, 78, 55), AnimationFrameType::Idle);
	anim_walkFWD.addFrame(sf::IntRect(78, 55, 78, 55), AnimationFrameType::Idle);
	anim_walkFWD.addFrame(sf::IntRect(156, 55, 78, 55), AnimationFrameType::Idle);
	anim_walkFWD.addFrame(sf::IntRect(234, 55, 78, 55), AnimationFrameType::Idle);
	anim_walkFWD.addFrame(sf::IntRect(312, 55, 78, 55), AnimationFrameType::Idle);
	anim_walkFWD.addFrame(sf::IntRect(390, 55, 78, 55), AnimationFrameType::Idle);
	anim_walkFWD.addFrame(sf::IntRect(0, 110, 78, 55), AnimationFrameType::Idle);
	anim_walkFWD.setFrameSpeed(0.15f);

	//fastPunch.addFrame(sf::IntRect(156, 165, 78, 55), AnimationFrameType::StartUp);
	anim_fastPunch.addFrame(sf::IntRect(234, 165, 78, 55), AnimationFrameType::StartUp);
	anim_fastPunch.addFrame(sf::IntRect(312, 165, 78, 55), AnimationFrameType::Active);
	anim_fastPunch.addFrame(sf::IntRect(312, 165, 78, 55), AnimationFrameType::Active);
	anim_fastPunch.addFrame(sf::IntRect(312, 165, 78, 55), AnimationFrameType::Recovery);
	anim_fastPunch.setFrameSpeed(0.1f);
	anim_fastPunch.setLooping(false);

	anim_defend.addFrame(sf::IntRect(0, 165, 78, 55), AnimationFrameType::StartUp);
	anim_defend.setLooping(false);

	currentAnim = &anim_idle;
}
