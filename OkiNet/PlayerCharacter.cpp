#include "PlayerCharacter.h"

PlayerCharacter::PlayerCharacter()
{
	// Set character defaults
	playerID = PlayerID::PlayerOne;
	playerState = PlayerState::Alive; // Alive by default
	moveState = MoveState::Idle; // Idle by default
	attackState = AttackState::None;
	animState = AnimationFrameType::Idle;

	shouldAcceptInput = true;

	b_dashTriggerL = false; // Helps checking if player can/wants to dash
	b_dashTriggerR = false;
	dashTimer = 0.0f; // Helper counter
	dashTime = 0.7f;

	b_fastPunch = false;

	grounded = false; //Sets the player able to jump when true
	CanGoLeft = true; //Allows the player to move left 
	CanGoRight = true;
	CanGoUp = true; //If theres a ceiling, doesnt allow to go up
	maxHealthPoints = 50; //Total hit points the player can suffer before dying
	currentHealthPoints = 50; //Current health 
	MoveSpeed = 180; //Multiplier for the movement speed
	dashDistance = 5000;
	currentAnim = NULL;
	CharacterSetUp = false;
	bodyColl = new CollisionBox();
	punchColl = new CollisionBox();

}

PlayerCharacter::~PlayerCharacter()
{
	currentAnim = NULL;
	delete currentAnim;

	bodyColl = NULL;
	delete bodyColl;

	punchColl = NULL;
	delete punchColl;
}

/// <summary>
/// Initialize the player character. Must be called at scene initialization to setup different players
/// </summary>
/// <param name="id"></param> Whether this is player one or two
/// <param name="startPos"></param> Spawn location in the world
void PlayerCharacter::InitCharacter(PlayerID id, sf::Vector2f startPos)
{
	playerID = id;

	// Assign the corresponding graphics to each player 
	if (id == PlayerID::PlayerOne)
	{
		// Configure player one graphics
		texture.loadFromFile("Sprites/Player1_Sheet.png");
		setSize(sf::Vector2f(78, 55));

		setScale(static_cast<float>(PIXEL_SCALE_FACTOR), static_cast<float>(PIXEL_SCALE_FACTOR));
		setTexture(&texture);
	}
	else
	{
		// Configure player two graphics
		texture.loadFromFile("Sprites/Player2_Sheet.png");
		setSize(sf::Vector2f(78, 55));
		setScale(static_cast<float>(PIXEL_SCALE_FACTOR), static_cast<float>(PIXEL_SCALE_FACTOR));
		setTexture(&texture);
		flipped = true;
	}

	// Setup Animations
	SetUpAnimations();

	setPosition(startPos);

	// Setup collision
	// Body collision
	bodyCollOffset = sf::Vector2f(static_cast <float>(25 * PIXEL_SCALE_FACTOR), static_cast <float>(5 * PIXEL_SCALE_FACTOR));
	if (flipped) bodyCollOffset += sf::Vector2f(15, 0); // Theres an issue when flipping the colliders that will offset them for an amount, correct it adding this to the flip colliders
	sf::Vector2f bodycallPos = getPosition() + bodyCollOffset;
	sf::Vector2f bodycallSize = sf::Vector2f(static_cast <float>(25 * PIXEL_SCALE_FACTOR), static_cast <float>(42 * PIXEL_SCALE_FACTOR));
	bodyColl = new CollisionBox(CollisionBox::ColliderType::HurtBox, sf::Color::Transparent, sf::Color::Green, 5.0f, bodycallPos, bodycallSize);
	// Punch collision



	CharacterSetUp = true;
}



//Manages the movement and animation of the player
void PlayerCharacter::update(float dt, sf::Window* wnd)
{
	// Update collider
	sf::Vector2f newPos = getPosition() + bodyCollOffset;
	bodyColl->setPosition(newPos);

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

		// Defend ----------
		if (input->isKeyDown(sf::Keyboard::S))
		{
			attackState = AttackState::Defend;
			animState = AnimationFrameType::StartUp;
		}
		//-------------------
		// Attacks ----------
		if (input->isKeyDown(sf::Keyboard::Q))
		{
			input->SetKeyUp(sf::Keyboard::Q); // Lift key so it acts as trigger
			attackState = AttackState::FastPunch;
			animState = AnimationFrameType::StartUp;
		}
		//-------------------
		// Movement ---------
		if (input->isKeyDown(sf::Keyboard::A) && CanGoLeft) // Left
		{
			if (b_dashTriggerL) // Dash 
			{
				dashTimer = dashTime;
				setPosition(getPosition().x - dashDistance * dt, getPosition().y);
				moveState = MoveState::DashL;
			}
			else // Walk
			{
				setPosition(getPosition().x - MoveSpeed * dt, getPosition().y);
				moveState = MoveState::Left;
			}

		}
		else if (input->isKeyDown(sf::Keyboard::D) && CanGoRight) // Right
		{
			if (b_dashTriggerR) // Dash 
			{
				dashTimer = dashTime;
				setPosition(getPosition().x + dashDistance * dt, getPosition().y);
				moveState = MoveState::DashR;
			}
			else // Walk
			{
				setPosition(getPosition().x + MoveSpeed * dt, getPosition().y);
				moveState = MoveState::Right;
			}

		}
		else // idle
		{
			// if last move state was walking, check the dash relevant trigger
			if (moveState == MoveState::Left)
			{
				b_dashTriggerL = true;
				dashTimer = 0.0f;
			}
			if (moveState == MoveState::Right)
			{
				b_dashTriggerR = true;
				dashTimer = 0.0f;
			}

			moveState = MoveState::Idle;
		}
		// -------------------
		// Check timers and counters
		if (dashTimer >= dashTime) // Check dashing timer
		{
			b_dashTriggerR = false;
			b_dashTriggerL = false;
			dashTimer = 0.0f;
		}
		else if (b_dashTriggerL || b_dashTriggerR) dashTimer += 0.1f;

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
		if (currentAnim->IsAnimationCompleted() && animState == AnimationFrameType::StartUp) // TODO: modify condition so that it checks if this is the loop where we inputted the action
		{
			anim_fastPunch.ResetAnimation();
			currentAnim = &anim_fastPunch;
		}
		else
			if (currentAnim->IsAnimationCompleted())
			{
				anim_fastPunch.ResetAnimation();
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
			// Reset non-looping anims
			if (anim_dashBKW.IsAnimationCompleted())	anim_dashBKW.ResetAnimation();
			if (anim_dashFWD.IsAnimationCompleted())	anim_dashFWD.ResetAnimation();

			currentAnim = &anim_idle;
			break;
		case MoveState::Right:
			currentAnim = &anim_walkFWD;
			break;
		case MoveState::Left:
			currentAnim = &anim_walkFWD;
			break;
		case MoveState::DashL:
			currentAnim = &anim_dashBKW;
			break;
		case MoveState::DashR:
			currentAnim = &anim_dashFWD;
			break;
		default:
			break;
		}
	}

	// Remember the animation class of the flip state of the sprite
	if (flipped)
	{
		currentAnim->SetFlipped(true);
	}


	currentAnim->Animate(dt);
	setTextureRect(currentAnim->GetCurrentFrame().GetRect());
	setFillColor(sf::Color(getFillColor().r, getFillColor().g, getFillColor().b, 250));

	// Set the animation state (startup, active, recovery...)
	animState = currentAnim->GetCurrentFrame().GetFrameType();
}

void PlayerCharacter::CollisionResponseToPlayer(Collision::CollisionResponse* collResponse)
{
	// Block movement depending on the side collided
	if (collResponse->s1Right)
	{
		CanGoRight = false;
	}
	else CanGoRight = true;

	if (collResponse->s1Left)
	{
		CanGoLeft = false;
	}
	else CanGoLeft = true;

}

void PlayerCharacter::NoCollisionRegistered()
{
	// Reset conditions based on collision
	if (!CanGoRight) CanGoRight = true;
	if (!CanGoLeft) CanGoLeft = true;

}

void PlayerCharacter::SetUpAnimations()
{
	anim_idle.AddFrame(sf::IntRect(0, 0, 78, 55), AnimationFrameType::Idle);
	anim_idle.AddFrame(sf::IntRect(78, 0, 78, 55), AnimationFrameType::Idle);
	anim_idle.AddFrame(sf::IntRect(156, 0, 78, 55), AnimationFrameType::Idle);
	anim_idle.AddFrame(sf::IntRect(234, 0, 78, 55), AnimationFrameType::Idle);
	anim_idle.SetFrameSpeed(0.1f);

	anim_walkFWD.AddFrame(sf::IntRect(0, 55, 78, 55), AnimationFrameType::Idle);
	anim_walkFWD.AddFrame(sf::IntRect(78, 55, 78, 55), AnimationFrameType::Idle);
	anim_walkFWD.AddFrame(sf::IntRect(156, 55, 78, 55), AnimationFrameType::Idle);
	anim_walkFWD.AddFrame(sf::IntRect(234, 55, 78, 55), AnimationFrameType::Idle);
	anim_walkFWD.AddFrame(sf::IntRect(312, 55, 78, 55), AnimationFrameType::Idle);
	anim_walkFWD.AddFrame(sf::IntRect(390, 55, 78, 55), AnimationFrameType::Idle);
	anim_walkFWD.AddFrame(sf::IntRect(0, 110, 78, 55), AnimationFrameType::Idle);
	anim_walkFWD.SetFrameSpeed(0.1f);

	//fastPunch.addFrame(sf::IntRect(156, 165, 78, 55), AnimationFrameType::StartUp);
	anim_fastPunch.AddFrame(sf::IntRect(234, 165, 78, 55), AnimationFrameType::StartUp);
	anim_fastPunch.AddFrame(sf::IntRect(312, 165, 78, 55), AnimationFrameType::Active);
	anim_fastPunch.AddFrame(sf::IntRect(312, 165, 78, 55), AnimationFrameType::Active);
	anim_fastPunch.AddFrame(sf::IntRect(312, 165, 78, 55), AnimationFrameType::Recovery);
	anim_fastPunch.SetFrameSpeed(0.1f);
	anim_fastPunch.SetLooping(false);

	anim_defend.AddFrame(sf::IntRect(0, 165, 78, 55), AnimationFrameType::StartUp);
	anim_defend.SetLooping(false);

	anim_dashFWD.AddFrame(sf::IntRect(156, 110, 78, 55), AnimationFrameType::StartUp);
	anim_dashFWD.AddFrame(sf::IntRect(234, 110, 78, 55), AnimationFrameType::StartUp);
	anim_dashFWD.AddFrame(sf::IntRect(312, 110, 78, 55), AnimationFrameType::Recovery);
	anim_dashFWD.SetLooping(false);
	anim_dashFWD.SetFrameSpeed(0.1f);

	anim_dashBKW.AddFrame(sf::IntRect(390, 110, 78, 55), AnimationFrameType::StartUp);
	anim_dashBKW.AddFrame(sf::IntRect(312, 110, 78, 55), AnimationFrameType::StartUp);
	anim_dashBKW.AddFrame(sf::IntRect(312, 110, 78, 55), AnimationFrameType::Recovery);
	anim_dashBKW.SetLooping(false);
	anim_dashBKW.SetFrameSpeed(0.1f);

	currentAnim = &anim_idle;
}
