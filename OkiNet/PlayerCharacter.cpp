#include "PlayerCharacter.h"

PlayerCharacter::PlayerCharacter()
{
	// Set character defaults
	playerID = PlayerID::PlayerOne;
	playerState = PlayerState::Alive; // Alive by default
	moveState = MoveState::Idle; // Idle by default
	attackState = AttackState::None;
	animState = AnimationFrameType::Idle;
	networkAuthority = NetworkAuthority::Offline; // ofline by default

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

/// Initialize the player character. Must be called at scene initialization to setup different players
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

void PlayerCharacter::InitNetworkedCharacter(PlayerID id, sf::Vector2f startPos, std::shared_ptr<NetworkPeer> peerRef, bool isLocalCharacter)
{
	// Assign the network object
	thisPeer = peerRef;
	playerID = id;
	isLocalCharacter ? networkAuthority = NetworkAuthority::Local : networkAuthority = NetworkAuthority::Remote;

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
	// Update network component
	if (networkAuthority == NetworkAuthority::Local)
	{
		if (thisPeer->IsConnected())
		{
			thisPeer->Update();
		}

	}

	if (receivedDamage)
	{
		PushPlayer(sf::Vector2f(static_cast<float>(smallPushDistance), 0), dt);
		attackState = AttackState::HitStun;
	}
	else if (hitGuardBox)
	{
		PushPlayer(sf::Vector2f(static_cast<float>(smallPushDistance), 0), dt);
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
	// If this is the second local networked character use player 1 scheme for both players
	if (playerID == PlayerID::PlayerOne || playerID == PlayerID::PlayerTwo && networkAuthority == NetworkAuthority::Local)
	{
		// Send ping request for roundtrip time
		if (input->IsKeyDown(sf::Keyboard::P))
		{
			input->SetKeyUp(sf::Keyboard::P);
			thisPeer->PingRequest();
		}

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
		if (input->IsKeyDown(sf::Keyboard::S))
		{
			attackState = AttackState::Defend;
			shouldAcceptInput = false;

			if (networkAuthority == NetworkAuthority::Local) thisPeer->Pressed_S();
		}

		if (!shouldAcceptInput) return;

		// Attack
		attackState = AttackState::None;
		// Special punch
		if (input->IsKeyDown(sf::Keyboard::Q) && input->IsKeyDown(sf::Keyboard::E))
		{
			input->SetKeyUp(sf::Keyboard::Q); // Lift key so it acts as trigger
			input->SetKeyUp(sf::Keyboard::E); // Lift key so it acts as trigger
			attackState = AttackState::DragonPunch;
		}else
		// Punch
		if (input->IsKeyDown(sf::Keyboard::Q) && input->IsKeyDown(sf::Keyboard::D))
		{
			input->SetKeyUp(sf::Keyboard::Q); // Lift key so it acts as trigger
			attackState = AttackState::HeavyPunch;

		}else if (input->IsKeyDown(sf::Keyboard::Q))
		{
			input->SetKeyUp(sf::Keyboard::Q); // Lift key so it acts as trigger
			attackState = AttackState::FastPunch;

			if (networkAuthority == NetworkAuthority::Local) thisPeer->Pressed_Q();
		}
		// Kick
		if (input->IsKeyDown(sf::Keyboard::E) && input->IsKeyDown(sf::Keyboard::D))
		{
			input->SetKeyUp(sf::Keyboard::E); // Lift key so it acts as trigger
			attackState = AttackState::HeavyKick;

		}
		else if (input->IsKeyDown(sf::Keyboard::E))
		{
			input->SetKeyUp(sf::Keyboard::E); // Lift key so it acts as trigger
			attackState = AttackState::FastKick;

		}

		//-------------------
		// Movement ---------
		if (input->IsKeyDown(sf::Keyboard::A) && CanGoLeft) // Left
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
			if (networkAuthority == NetworkAuthority::Local) thisPeer->Pressed_A();
		}
		else if (input->IsKeyDown(sf::Keyboard::D) && CanGoRight) // Right
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
			if (networkAuthority == NetworkAuthority::Local) thisPeer->Pressed_D();
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
		if (input->IsKeyDown(sf::Keyboard::Down))
		{
			attackState = AttackState::Defend;
			shouldAcceptInput = false;
		}

		if (!shouldAcceptInput) return;

		// Attack
		if (input->IsKeyDown(sf::Keyboard::Numpad0) && input->IsKeyDown(sf::Keyboard::Left))
		{
			input->SetKeyUp(sf::Keyboard::Numpad0); // Lift key so it acts as trigger
			attackState = AttackState::HeavyPunch;

		}
		else if (input->IsKeyDown(sf::Keyboard::Numpad0))
		{
			input->SetKeyUp(sf::Keyboard::Numpad0); // Lift key so it acts as trigger
			attackState = AttackState::FastPunch;
		}
		else attackState = AttackState::None;


		//-------------------
		// Movement ---------
		if (input->IsKeyDown(sf::Keyboard::Left) && CanGoLeft) // Left
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
		else if (input->IsKeyDown(sf::Keyboard::Right) && CanGoRight) // Right
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

void PlayerCharacter::HandleRemotePlayerInput(InputManager* input, float dt)
{
	if (networkAuthority == NetworkAuthority::Remote)
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
		if (thisPeer->remotePlayerStatus.Pressed_S)
		{
			attackState = AttackState::Defend;
			shouldAcceptInput = false;
		}

		if (shouldAcceptInput)
		{

			// Attack
			if (thisPeer->remotePlayerStatus.Pressed_Q)
			{
				attackState = AttackState::FastPunch;
			}
			else attackState = AttackState::None;
			//-------------------
			// Movement ---------
			if (thisPeer->remotePlayerStatus.Pressed_A && CanGoLeft) // Left
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
			else if (thisPeer->remotePlayerStatus.Pressed_D && CanGoRight) // Right
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
		}
	}
	// Resets all inputs this frame
	thisPeer->ResetRemotePlayerStatus();
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
		anim_heavyPunch.ResetAnimation();
		anim_fastkick.ResetAnimation();
		anim_heavyKick.ResetAnimation();
		anim_dragonPunch.ResetAnimation();
		anim_hurt.ResetAnimation();
		isAttacking = false;
		break;
	case AttackState::FastPunch:
		currentAnim = &anim_fastPunch;
		break;
	case AttackState::HeavyPunch:
		//if (currentAnim->GetCurrentFrame().GetFrameType() == AnimationFrameType::StartUp)
		//{
		//	PushPlayer(sf::Vector2f(static_cast<float>(-18), 0), dt * 5);
		//}
		currentAnim = &anim_heavyPunch;
		break;
	case AttackState::FastKick:
		currentAnim = &anim_fastkick;
		break;
	case AttackState::HeavyKick:
		currentAnim = &anim_heavyKick;
		break;
	case AttackState::DragonPunch:
		currentAnim = &anim_dragonPunch;
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
			flipped ? currentAnim = &anim_dashFWD : currentAnim = &anim_dashBKW;
			break;
		case MoveState::DashR:
			flipped ? currentAnim = &anim_dashBKW : currentAnim = &anim_dashFWD;
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
		}
		else inflictedDamage = false;

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

	// Idle -----
	anim_idle.AddFrame(sf::IntRect(0, 0, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_idle.AddFrame(sf::IntRect(78, 0, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_idle.AddFrame(sf::IntRect(156, 0, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_idle.AddFrame(sf::IntRect(234, 0, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_idle.SetFrameSpeed(0.1f);

	// Walk -----
	anim_walkFWD.AddFrame(sf::IntRect(0, 55, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_walkFWD.AddFrame(sf::IntRect(78, 55, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_walkFWD.AddFrame(sf::IntRect(156, 55, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_walkFWD.AddFrame(sf::IntRect(234, 55, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_walkFWD.AddFrame(sf::IntRect(312, 55, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_walkFWD.AddFrame(sf::IntRect(390, 55, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_walkFWD.AddFrame(sf::IntRect(0, 110, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_walkFWD.SetFrameSpeed(0.1f);

	// Defend ---
	CollisionBox* GuardColl = new CollisionBox(CollisionBox::ColliderType::GuardBox, bodycallPos, bodycallSize, bodyCollOffset);
	anim_defend.AddFrame(sf::IntRect(0, 165, 78, 55), AnimationFrameType::Idle, *GuardColl);
	anim_defend.SetLooping(false);

	// Dash FWD and BKW ---
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

	// Hurt anim -----
	anim_hurt.AddFrame(sf::IntRect(78, 165, 78, 55), AnimationFrameType::Active, *bodyColl);
	anim_hurt.AddFrame(sf::IntRect(156, 165, 78, 55), AnimationFrameType::Active, *bodyColl);
	anim_hurt.AddFrame(sf::IntRect(234, 165, 78, 55), AnimationFrameType::Active, *bodyColl);
	anim_hurt.AddFrame(sf::IntRect(234, 165, 78, 55), AnimationFrameType::Recovery, *bodyColl);
	anim_hurt.SetLooping(false);
	anim_hurt.SetFrameSpeed(0.1f);


	// Fast Punch attack ------
	sf::Vector2f  punchCollOffset = sf::Vector2f(static_cast <float>(15 * PIXEL_SCALE_FACTOR), static_cast <float>(-7 * PIXEL_SCALE_FACTOR));
	sf::Vector2f punchCollPos = getPosition() + punchCollOffset;
	sf::Vector2f punchCollSize = sf::Vector2f(static_cast <float>(17 * PIXEL_SCALE_FACTOR), static_cast <float>(10 * PIXEL_SCALE_FACTOR));
	CollisionBox* punchColl = new CollisionBox(CollisionBox::ColliderType::HitBox, punchCollPos, punchCollSize, punchCollOffset);

	std::vector<CollisionBox*> punchCollVector;
	punchCollVector.push_back(bodyColl);
	punchCollVector.push_back(punchColl);

	anim_fastPunch.AddFrame(sf::IntRect(312, 165, 78, 55), AnimationFrameType::StartUp, *bodyColl);
	anim_fastPunch.AddFrame(sf::IntRect(0, 220, 78, 55), AnimationFrameType::Active, punchCollVector);
	anim_fastPunch.AddFrame(sf::IntRect(0, 220, 78, 55), AnimationFrameType::Active, punchCollVector);
	anim_fastPunch.AddFrame(sf::IntRect(78, 220, 78, 55), AnimationFrameType::Recovery, *bodyColl);
	anim_fastPunch.SetFrameSpeed(0.1f);
	anim_fastPunch.SetLooping(false);

	// Heavy punch 
	sf::Vector2f hpunchCollOffset = sf::Vector2f(static_cast <float>(15 * PIXEL_SCALE_FACTOR), static_cast <float>(-7 * PIXEL_SCALE_FACTOR));
	sf::Vector2f hpunchCollPos = getPosition() + hpunchCollOffset;
	sf::Vector2f hpunchCollSize = sf::Vector2f(static_cast <float>(22 * PIXEL_SCALE_FACTOR), static_cast <float>(10 * PIXEL_SCALE_FACTOR));
	CollisionBox* hpunchColl = new CollisionBox(CollisionBox::ColliderType::HitBox, hpunchCollPos, hpunchCollSize, hpunchCollOffset);

	std::vector<CollisionBox*> hpunchCollVector;
	hpunchCollVector.push_back(bodyColl);
	hpunchCollVector.push_back(hpunchColl);

	anim_heavyPunch.AddFrame(sf::IntRect(78, 220, 78, 55), AnimationFrameType::StartUp, *bodyColl);
	anim_heavyPunch.AddFrame(sf::IntRect(156, 220, 78, 55), AnimationFrameType::StartUp, *bodyColl);
	anim_heavyPunch.AddFrame(sf::IntRect(234, 220, 78, 55), AnimationFrameType::Active, hpunchCollVector);
	anim_heavyPunch.AddFrame(sf::IntRect(234, 220, 78, 55), AnimationFrameType::Active, hpunchCollVector);
	anim_heavyPunch.AddFrame(sf::IntRect(309, 220, 78, 55), AnimationFrameType::Recovery, *bodyColl);
	anim_heavyPunch.SetFrameSpeed(0.1f);
	anim_heavyPunch.SetLooping(false);

	// fast kick
	anim_fastkick.AddFrame(sf::IntRect(0, 275, 78, 55), AnimationFrameType::StartUp);
	anim_fastkick.AddFrame(sf::IntRect(78, 275, 78, 55), AnimationFrameType::Active);
	anim_fastkick.AddFrame(sf::IntRect(78, 275, 78, 55), AnimationFrameType::Active);
	anim_fastkick.AddFrame(sf::IntRect(156, 275, 78, 55), AnimationFrameType::Recovery);
	anim_fastkick.SetFrameSpeed(0.1f);
	anim_fastkick.SetLooping(false);

	// Heavy kick
	anim_heavyKick.AddFrame(sf::IntRect(234, 275, 78, 55), AnimationFrameType::StartUp);
	anim_heavyKick.AddFrame(sf::IntRect(309, 275, 78, 55), AnimationFrameType::StartUp);
	anim_heavyKick.AddFrame(sf::IntRect(387, 275, 78, 55), AnimationFrameType::StartUp);
	anim_heavyKick.AddFrame(sf::IntRect(0, 330, 78, 55), AnimationFrameType::Active);
	anim_heavyKick.AddFrame(sf::IntRect(78, 330, 78, 55), AnimationFrameType::Recovery);
	anim_heavyKick.SetFrameSpeed(0.1f);
	anim_heavyKick.SetLooping(false);

	// Dragon punch
	anim_dragonPunch.AddFrame(sf::IntRect(156, 330, 78, 55), AnimationFrameType::StartUp);
	anim_dragonPunch.AddFrame(sf::IntRect(234, 330, 78, 55), AnimationFrameType::StartUp);
	anim_dragonPunch.AddFrame(sf::IntRect(309, 330, 78, 55), AnimationFrameType::Active);
	anim_dragonPunch.AddFrame(sf::IntRect(387, 330, 78, 55), AnimationFrameType::Active);
	anim_dragonPunch.AddFrame(sf::IntRect(0, 385, 78, 55), AnimationFrameType::Active);
	anim_dragonPunch.AddFrame(sf::IntRect(78, 385, 78, 55), AnimationFrameType::Recovery);
	anim_dragonPunch.SetFrameSpeed(0.1f);
	anim_dragonPunch.SetLooping(false);

	// Die
	anim_die.AddFrame(sf::IntRect(156, 385, 78, 55), AnimationFrameType::Active);
	anim_die.AddFrame(sf::IntRect(234, 385, 78, 55), AnimationFrameType::Active);
	anim_die.AddFrame(sf::IntRect(309, 385, 78, 55), AnimationFrameType::Active);
	anim_die.AddFrame(sf::IntRect(387, 385, 78, 55), AnimationFrameType::Active);
	anim_die.SetFrameSpeed(0.1f);
	anim_die.SetLooping(false);

	// Initialize to idle
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
