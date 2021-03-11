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
	CharacterSetUp = false;

	b_dashTriggerL = false; // Helps checking if player can/wants to dash
	b_dashTriggerR = false;
	dashTimer = 0.0f; // Helper counter
	dashTime = 0.7f;

	grounded = false; //Sets the player able to jump when true
	CanGoLeft = true; //Allows the player to move left 
	CanGoRight = true;

	hitGuardBox = false;
	inflictedDamage = false;
	receivedDamage = false;

	b_fastPunch = false;
	currentAnim = NULL;

	maxHealthPoints = 50; //Total hit points the player can suffer before dying
	currentHealthPoints = 50; //Current health 

	moveDistance = 180;
	dashDistance = 5000;
	smallPushDistance = 180;

}

PlayerCharacter::~PlayerCharacter()
{
	currentAnim = NULL;
	delete currentAnim;
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
		setOrigin(sf::Vector2f(78 / 2, 55 / 2));
		setScale(static_cast<float>(PIXEL_SCALE_FACTOR), static_cast<float>(PIXEL_SCALE_FACTOR));
		setTexture(&texture);
	}
	else
	{
		// Configure player two graphics
		texture.loadFromFile("Sprites/Player2_Sheet.png");
		setSize(sf::Vector2f(78, 55));
		setOrigin(sf::Vector2f(78 / 2, 55 / 2));
		setScale(static_cast<float>(PIXEL_SCALE_FACTOR), static_cast<float>(PIXEL_SCALE_FACTOR));
		setTexture(&texture);
		flipped = true;
	}

	// Position character at designated start position
	setPosition(startPos);
	// Setup Animations
	SetUpAnimationFrames();

	CharacterSetUp = true;
}



//Manages the movement and animation of the player
void PlayerCharacter::Update(float dt, sf::Window* wnd)
{
	if (receivedDamage)
	{
		PushPlayer(sf::Vector2f(smallPushDistance, 0), dt);
		attackState = AttackState::HitStun;
	}

	HandleAnimation(dt);

	// Position the colliders in animation 
	for (auto coll : GetCurrentCollision())
	{
		coll->SetCollisionBoxPosition(getPosition());
	}
}

void PlayerCharacter::HandleInput(InputManager* input, float dt)
{
	if (playerID == PlayerID::PlayerOne)
	{
		// Check timers and counters
		if (dashTimer >= dashTime) // Check dashing timer
		{
			b_dashTriggerR = false;
			b_dashTriggerL = false;
			dashTimer = 0.0f;
		}
		else if (b_dashTriggerL || b_dashTriggerR) dashTimer += 0.1f;

		// Check this frame's type to decide if input should be accepted
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

		// Defend
		if (input->isKeyDown(sf::Keyboard::S))
		{
			attackState = AttackState::Defend;
			shouldAcceptInput = false;
		}

		if (!shouldAcceptInput) return;

		// Attack
		if (input->isKeyDown(sf::Keyboard::Q))
		{
			input->SetKeyUp(sf::Keyboard::Q); // Lift key so it acts as trigger
			attackState = AttackState::FastPunch;
		}
		else attackState = AttackState::None;
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
				setPosition(getPosition().x - moveDistance * dt, getPosition().y);
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
				setPosition(getPosition().x + moveDistance * dt, getPosition().y);
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

	}
	else if (playerID == PlayerID::PlayerTwo)
	{
		// Check timers and counters
		if (dashTimer >= dashTime) // Check dashing timer
		{
			b_dashTriggerR = false;
			b_dashTriggerL = false;
			dashTimer = 0.0f;
		}
		else if (b_dashTriggerL || b_dashTriggerR) dashTimer += 0.1f;

		// Check this frame's type to decide if input should be accepted
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

		// Defend
		if (input->isKeyDown(sf::Keyboard::Down))
		{
			attackState = AttackState::Defend;
			shouldAcceptInput = false;
		}

		if (!shouldAcceptInput) return;

		// Attack
		if (input->isKeyDown(sf::Keyboard::Numpad0))
		{
			input->SetKeyUp(sf::Keyboard::Numpad0); // Lift key so it acts as trigger
			attackState = AttackState::FastPunch;
		}
		else attackState = AttackState::None;
		//-------------------
		// Movement ---------
		if (input->isKeyDown(sf::Keyboard::Left) && CanGoLeft) // Left
		{
			if (b_dashTriggerL) // Dash 
			{
				dashTimer = dashTime;
				setPosition(getPosition().x - dashDistance * dt, getPosition().y);
				moveState = MoveState::DashL;
			}
			else // Walk
			{
				setPosition(getPosition().x - moveDistance * dt, getPosition().y);
				moveState = MoveState::Left;
			}

		}
		else if (input->isKeyDown(sf::Keyboard::Right) && CanGoRight) // Right
		{
			if (b_dashTriggerR) // Dash 
			{
				dashTimer = dashTime;
				setPosition(getPosition().x + dashDistance * dt, getPosition().y);
				moveState = MoveState::DashR;
			}
			else // Walk
			{
				setPosition(getPosition().x + moveDistance * dt, getPosition().y);
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
	}


}

void PlayerCharacter::HandleAnimation(float dt)
{
	// Remember the animation class of the flip state of the sprite
	if (flipped)
	{
		currentAnim->SetFlipped(true);
	}

	// ATTACK --------------------------------------------------------
	bool isAttacking = true;
	// Check the attack state, if any
	switch (attackState)
	{
	case AttackState::None:
		anim_fastPunch.ResetAnimation();
		isAttacking = false;
		break;
	case AttackState::FastPunch:
		currentAnim = &anim_fastPunch;
		break;
	case AttackState::Defend:
		currentAnim = &anim_defend;
		break;
	case AttackState::HitStun:
		currentAnim = &anim_hurt;
		break;
	case AttackState::BlockStun:
		currentAnim = &anim_defend;
		break;
	default:
		break;
	}

	// MOVE - Calculate the move state if we are not attacking
	if (!isAttacking)
	{
		// Update movement animation
		switch (moveState)
		{
		case MoveState::Idle:
			// Reset non-looping anims
			anim_dashBKW.ResetAnimation();
			anim_dashFWD.ResetAnimation();
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

	setTextureRect(currentAnim->GetCurrentFrame().GetRect()); // Set the part of the sprite sheet to draw
	setFillColor(sf::Color(getFillColor().r, getFillColor().g, getFillColor().b, 250)); // Set the color information of the rect to fill it
	animState = currentAnim->GetCurrentFrame().GetFrameType(); // Set the animation state (startup, active, recovery...)
	currentAnim->Animate(dt); // Set to advance frames
}

void PlayerCharacter::CollisionResponseToPlayer(Collision::CollisionResponse* collResponse)
{
	if (playerID == PlayerID::PlayerOne)
	{
		// Block movement on hurtbox collision depending on the side collided
		// Check movement against a guard
		if (collResponse->s1CollType == CollisionBox::ColliderType::HurtBox && collResponse->s2CollType == CollisionBox::ColliderType::HurtBox
			|| collResponse->s1CollType == CollisionBox::ColliderType::HurtBox && collResponse->s2CollType == CollisionBox::ColliderType::GuardBox)
		{
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

		// Check for inflict damage collision
		if (collResponse->s1CollType == CollisionBox::ColliderType::HitBox && collResponse->s2CollType == CollisionBox::ColliderType::HurtBox)
		{
			inflictedDamage = true;
		}else inflictedDamage = false;

		// Check for suffering damage
		if (collResponse->s1CollType == CollisionBox::ColliderType::HurtBox && collResponse->s2CollType == CollisionBox::ColliderType::HitBox)
		{
			receivedDamage = true;
		}
		else receivedDamage = false;

		// Check hitting guard box
		if (collResponse->s1CollType == CollisionBox::ColliderType::HurtBox && collResponse->s2CollType == CollisionBox::ColliderType::GuardBox)
		{
			hitGuardBox = true;
		}
		else hitGuardBox = false;
	}
	else
	{
		// Block movement on hurtbox collision depending on the side collided
		// Check movement against a guard
		if (collResponse->s1CollType == CollisionBox::ColliderType::HurtBox && collResponse->s2CollType == CollisionBox::ColliderType::HurtBox
			|| collResponse->s2CollType == CollisionBox::ColliderType::HurtBox && collResponse->s1CollType == CollisionBox::ColliderType::GuardBox)
		{
			if (collResponse->s2Right)
			{
				CanGoRight = false;
			}
			else CanGoRight = true;

			if (collResponse->s2Left)
			{
				CanGoLeft = false;
			}
			else CanGoLeft = true;
		}
		// Check for inflict damage collision
		if (collResponse->s2CollType == CollisionBox::ColliderType::HitBox && collResponse->s1CollType == CollisionBox::ColliderType::HurtBox)
		{
			inflictedDamage = true;
		}
		else inflictedDamage = false;

		// Check for suffering damage
		if (collResponse->s2CollType == CollisionBox::ColliderType::HurtBox && collResponse->s1CollType == CollisionBox::ColliderType::HitBox)
		{
			receivedDamage = true;
		}
		else receivedDamage = false;

		// Check hitting guard box
		if (collResponse->s2CollType == CollisionBox::ColliderType::HurtBox && collResponse->s1CollType == CollisionBox::ColliderType::GuardBox)
		{
			hitGuardBox = true;
		}
		else hitGuardBox = false;
	}
}

void PlayerCharacter::NoCollisionRegistered()
{
	// Reset conditions based on absence of collision
	if (!CanGoRight) CanGoRight = true;
	if (!CanGoLeft) CanGoLeft = true;
	if (receivedDamage) receivedDamage = false;
	if (inflictedDamage) inflictedDamage = false;
	if (hitGuardBox) hitGuardBox = false;

}

void PlayerCharacter::SetUpAnimationFrames()
{
	// Body collision
	// Add an offset to account for the extra space not used as we start on the top left 0,0 corner
	sf::Vector2f bodyCollOffset = sf::Vector2f(static_cast <float>(0 * PIXEL_SCALE_FACTOR), static_cast <float>(0 * PIXEL_SCALE_FACTOR));
	sf::Vector2f bodycallPos = getPosition() + bodyCollOffset;
	sf::Vector2f bodycallSize = sf::Vector2f(static_cast <float>(15 * PIXEL_SCALE_FACTOR), static_cast <float>(42 * PIXEL_SCALE_FACTOR));
	CollisionBox* bodyColl = new CollisionBox(CollisionBox::ColliderType::HurtBox, bodycallPos, bodycallSize, bodyCollOffset);

	anim_idle.AddFrame(sf::IntRect(0, 0, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_idle.AddFrame(sf::IntRect(78, 0, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_idle.AddFrame(sf::IntRect(156, 0, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_idle.AddFrame(sf::IntRect(234, 0, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_idle.SetFrameSpeed(0.1f);


	anim_walkFWD.AddFrame(sf::IntRect(0, 55, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_walkFWD.AddFrame(sf::IntRect(78, 55, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_walkFWD.AddFrame(sf::IntRect(156, 55, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_walkFWD.AddFrame(sf::IntRect(234, 55, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_walkFWD.AddFrame(sf::IntRect(312, 55, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_walkFWD.AddFrame(sf::IntRect(390, 55, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_walkFWD.AddFrame(sf::IntRect(0, 110, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_walkFWD.SetFrameSpeed(0.1f);

	// Punch attack
	sf::Vector2f  punchCollOffset = sf::Vector2f(static_cast <float>(15 * PIXEL_SCALE_FACTOR), static_cast <float>(-7 * PIXEL_SCALE_FACTOR));
	sf::Vector2f punchCollPos = getPosition() + punchCollOffset;
	sf::Vector2f punchCollSize = sf::Vector2f(static_cast <float>(15 * PIXEL_SCALE_FACTOR), static_cast <float>(10 * PIXEL_SCALE_FACTOR));
	CollisionBox* punchColl = new CollisionBox(CollisionBox::ColliderType::HitBox, punchCollPos, punchCollSize, punchCollOffset);

	std::vector<CollisionBox*> punchCollVector;
	punchCollVector.push_back(bodyColl);
	punchCollVector.push_back(punchColl);

	// fastPunch.addFrame(sf::IntRect(156, 165, 78, 55), AnimationFrameType::StartUp);
	anim_fastPunch.AddFrame(sf::IntRect(234, 165, 78, 55), AnimationFrameType::StartUp, *bodyColl);
	anim_fastPunch.AddFrame(sf::IntRect(312, 165, 78, 55), AnimationFrameType::Active, punchCollVector);
	anim_fastPunch.AddFrame(sf::IntRect(312, 165, 78, 55), AnimationFrameType::Active, punchCollVector);
	anim_fastPunch.AddFrame(sf::IntRect(312, 165, 78, 55), AnimationFrameType::Recovery, *bodyColl);
	anim_fastPunch.SetFrameSpeed(0.1f);
	anim_fastPunch.SetLooping(false);

	CollisionBox* GuardColl = new CollisionBox(CollisionBox::ColliderType::GuardBox, bodycallPos, bodycallSize, bodyCollOffset);
	anim_defend.AddFrame(sf::IntRect(0, 165, 78, 55), AnimationFrameType::Idle, *GuardColl);
	anim_defend.SetLooping(false);

	anim_dashFWD.AddFrame(sf::IntRect(156, 110, 78, 55), AnimationFrameType::StartUp, *bodyColl);
	anim_dashFWD.AddFrame(sf::IntRect(234, 110, 78, 55), AnimationFrameType::StartUp, *bodyColl);
	anim_dashFWD.AddFrame(sf::IntRect(312, 110, 78, 55), AnimationFrameType::Recovery, *bodyColl);
	anim_dashFWD.SetLooping(false);
	anim_dashFWD.SetFrameSpeed(0.1f);

	anim_dashBKW.AddFrame(sf::IntRect(390, 110, 78, 55), AnimationFrameType::StartUp, *bodyColl);
	anim_dashBKW.AddFrame(sf::IntRect(312, 110, 78, 55), AnimationFrameType::StartUp, *bodyColl);
	anim_dashBKW.AddFrame(sf::IntRect(312, 110, 78, 55), AnimationFrameType::Recovery, *bodyColl);
	anim_dashBKW.SetLooping(false);
	anim_dashBKW.SetFrameSpeed(0.1f);

	anim_hurt.AddFrame(sf::IntRect(78, 165, 78, 55), AnimationFrameType::Active, *bodyColl);
	anim_hurt.AddFrame(sf::IntRect(78, 165, 78, 55), AnimationFrameType::Active, *bodyColl);
	anim_hurt.AddFrame(sf::IntRect(78, 165, 78, 55), AnimationFrameType::Active, *bodyColl);
	anim_hurt.AddFrame(sf::IntRect(78, 165, 78, 55), AnimationFrameType::Active, *bodyColl);
	anim_hurt.AddFrame(sf::IntRect(78, 165, 78, 55), AnimationFrameType::Active, *bodyColl);
	anim_hurt.AddFrame(sf::IntRect(78, 165, 78, 55), AnimationFrameType::Recovery, *bodyColl);
	anim_hurt.SetFrameSpeed(0.1f);
	currentAnim = &anim_idle;
}

void PlayerCharacter::PushPlayer(sf::Vector2f distance, float dt)
{
	if (flipped)
	{
		setPosition(getPosition() + distance * dt);
	}
	else setPosition(getPosition() - distance * dt);

}
