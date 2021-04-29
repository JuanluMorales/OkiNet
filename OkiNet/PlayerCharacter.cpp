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
	receivedGuardBox = false;

	b_fastPunch = false;
	currentAnim = NULL;

	currentHealthPoints = maxHealthPoints; //Current health 
	currentEnergyPoints = maxEnergyPoints;

	moveDistance = 3;
	dashDistance = 100;
	smallPushDistance = 2;

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
	// Use the dynamic delay or the fixed delay
	if (thisPeer->currentNetworkTechnique == NetworkTechnique::InputDelay) thisPeer->useDynamicDelay ? FRAME_DELAY = thisPeer->dynamicDelayFrames : FRAME_DELAY = thisPeer->ROLLBACK_DELAY_FRAMES;
	if (thisPeer->currentNetworkTechnique == NetworkTechnique::Rollback)  FRAME_DELAY = thisPeer->ROLLBACK_DELAY_FRAMES;

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
	if (isLocalCharacter)
	{
		thisPeer->localHP = currentHealthPoints;
		thisPeer->localPosX = static_cast<int>(getPosition().x);
	}
	else
	{
		thisPeer->remoteHP = currentHealthPoints;
		thisPeer->remotePosX = static_cast<int>(getPosition().x);
	}

	// Setup Animations
	SetUpAnimationFrames();

	CharacterSetUp = true;
}

void PlayerCharacter::UpdateNetworkState()
{
	thisPeer->Update();
}

//Manages the movement and animation of the player
void PlayerCharacter::Update(float dt, sf::Window* wnd, PlayerCharacter* playerTwo)
{
	// Update our info on the remote player state
	if (networkAuthority == NetworkAuthority::Remote)
	{
		// Pass the info on local image of the remote player state
		thisPeer->remoteHP = currentHealthPoints;
		thisPeer->remotePosX = static_cast<int>(getPosition().x);
	}
	if (networkAuthority == NetworkAuthority::Local)
	{
		thisPeer->localHP = currentHealthPoints;
		thisPeer->localPosX = static_cast<int>(getPosition().x);
	}

	// Update network component (send our state then listen)
	if (networkAuthority == NetworkAuthority::Local && thisPeer->IsConnected())
	{
		// Handle lockstep
		if (GetNetworkTechnique() == NetworkTechnique::DeterministicLockstep)
		{
			if (networkAuthority == NetworkAuthority::Local) thisPeer->ResetRemotePlayerStatus();

			// Send our state every frame
			thisPeer->SendPlayerStatus(thisPeer->localPlayerStatus);
			// IF we have not yet received update this frame, keep waiting by not updating or rendering			
			while (!HasReceivedRemoteUpdateThisFrame())
			{
				// Make sure we listen to the network messages
				UpdateNetworkState();
			}
		}
		else if (GetNetworkTechnique() == NetworkTechnique::InputDelay) // Handle input delay
		{
			// Use the dynamic delay or the fixed delay
			thisPeer->useDynamicDelay ? FRAME_DELAY = thisPeer->dynamicDelayFrames : FRAME_DELAY = thisPeer->ROLLBACK_DELAY_FRAMES;

			// Send the delayed status update
			if (!thisPeer->delayedPlayerStatuses.empty())
			{
				thisPeer->SendPlayerStatus(thisPeer->delayedPlayerStatuses.back());
			}

			// Force lockstep until we get new updates		
			int lockstepCount = 0;
			do
			{
				// Make sure we listen to the network messages
				UpdateNetworkState();

				if (!HasReceivedRemoteUpdateThisFrame())
				{
					if (lockstepCount == 0) remoteFrameWaitCounter += 1;
				}
				else
				{
					lockstepCount = 0;
					if (remoteFrameWaitCounter > 0) remoteFrameWaitCounter -= 1;
				}

				// If we ran out of waiting frames, enter lockstep -> loop while
				if (FRAME_DELAY < remoteFrameWaitCounter)
				{
					lockstepCount += 1;
				}

			} while (lockstepCount > 0);

			thisPeer->ResetRemotePlayerStatus();
		}
		else if (GetNetworkTechnique() == NetworkTechnique::Rollback)
		{
			// Send the delayed status update
			if (!thisPeer->delayedPlayerStatuses.empty())
			{
				thisPeer->SendPlayerStatus(thisPeer->delayedPlayerStatuses.back());
			}

			// We save the frame even if the remote data is wrong (we have no update from it) as we will later check for discrepancies with the new remote updates
			if (!thisPeer->delayedPlayerStatuses.empty() && !thisPeer->remoteDelayedPlayerStatuses.empty())
				thisPeer->Rollback_Save();

			// Check if we would enter lockstep, and then predict instead of block	
			int lockstepCount = 0;
			do
			{
				// Make sure we listen to the network messages
				UpdateNetworkState();

				if (!HasReceivedRemoteUpdateThisFrame())
				{
					remoteFrameWaitCounter += 1;
				}
				else
				{
					// If we have predicted any frames, we must rollback
					if (thisPeer->predictedRemoteStatuses.size() > 0)
					{
						std::cout << "Rollback!!!\n";
						//	// Rollback----
						// Find the first incorrect frame and restore the state to that frame
						int framesToResimulate = thisPeer->Rollback_Restore();

						std::cout << "Rolling back: " << framesToResimulate << " frames out of " << thisPeer->rollbackFrames.size() << " available rollback frames.\n";

						// Resimulate up to now again
						for (int i = 0; i < framesToResimulate; i++)
						{
							ResimulateFrame();
							playerTwo->ResimulateFrame();
						}
					}

					// Clear the predicted statuses as we no longer need them once we have the update
					thisPeer->predictedRemoteStatuses.clear();
					lockstepCount = 0;
					if (remoteFrameWaitCounter > 0) remoteFrameWaitCounter = FRAME_DELAY - 1;
				}

				// If we ran out of waiting frames, enter lockstep -> loop while
				if (FRAME_DELAY < remoteFrameWaitCounter)
				{
					// Assume whatever the player was doing, will keep doing (naive aproach) so we dont inmediately lockstep
					if (!thisPeer->remoteDelayedPlayerStatuses.empty()) // as long as we have actually received any message
					{
						thisPeer->Rollback_Predict();
						std::cout << "Predicted this frame\n";
						lockstepCount += 1; // add to the lockstep counter
					}
					else lockstepCount += 1;

				}

			} while (lockstepCount > thisPeer->ROLLBACK_FRAMES); // Only lockstep if we run out of both delay frames and rollback frames

			thisPeer->ResetRemotePlayerStatus();

			/*
			#pragma region Rollback
			// Save the frame state
						thisPeer->Rollback_Save(); // We save the frame even if the remote data is wrong (we have no update from it) as we will later check for discrepancies with the new remote updates

						// in order to not block we will predict what the remote will do based on their last input/s (if we dont have any messages we jump directly to lockstep)
						if (!HasReceivedRemoteUpdateThisFrame())
						{
							// Assume whatever the player was doing, will keep doing (naive aproach)
							thisPeer->Rollback_Predict();
						}

						// If we have predicted (it will probably be wrong) we need to rollback when we get updates from remote
						if (HasReceivedRemoteUpdateThisFrame() && thisPeer->predictedRemoteStatuses.size() > 0)
						{
							// Rollback----
							// Find the first incorrect frame and restore the state to that frame
							thisPeer->Rollback_Restore();

							int framesToResimulate = static_cast<int>(thisPeer->dynamicDelayFrames);
							std::cout << "Rolling back: " << framesToResimulate << " frames out of " << thisPeer->rollbackFrames.size() << " available rollback frames.\n";

							// Resimulate up to now again
							for (int i = 0; i < framesToResimulate; i++)
							{
								if(i = framesToResimulate - 1) thisPeer->localPlayerStatus = thisPeer->rollbackFrames.back().localStatus; // If we are using rollback, make sure to keep the input for this frame
								// do as many update cycles as needed to catch up/resimulate up to the current frame
								ResimulateFrame();
								playerTwo->ResimulateFrame();

							}

							// Clear the predicted statuses as we no longer need them
							thisPeer->predictedRemoteStatuses.clear();
						}
						else
						{
							ExecuteInput();
							playerTwo->ExecuteRemoteInput();
						}
			#pragma endregion
			*/


			//// Check if we received a remote player state message this frame
			//if (!HasReceivedRemoteUpdateThisFrame())
			//{
			//	// in order to not block we will predict what the remote will do based on their last input/s (if we dont have any messages we jump directly to lockstep)
			//	// Assume whatever the player was doing, will keep doing (naive aproach) 
			//	thisPeer->Rollback_Predict();
			//	// if we predicted more frames than our rollback frames budget or we did not receive any messages to predict with
			//	if (thisPeer->predictedRemoteStatuses.size() >= thisPeer->rollbackFrames.size())
			//	{
			//		// Enter lockstep
			//		do
			//		{
			//			std::cout << "Entered rollback lockstep!\n";
			//			UpdateNetworkState();
			//		} while (HasReceivedRemoteUpdateThisFrame());
			//		// When we roll out of lockstep, we must reconcile the predicted states with the received new state 
			//		// Rollback to the first incorrect frame
			//		// Update the local status for both players
			//		// TODO:
			//		// Clear the predictions
			//		thisPeer->predictedRemoteStatuses.clear();
			//	}
			//}
			//else
			//{
			//	// If we have predicted statuses, it means we need to rollback
			//	if (thisPeer->predictedRemoteStatuses.size() > 0)
			//	{
			//		std::cout << "Rollback in progress. Rolling back: " << thisPeer->predictedRemoteStatuses.size() << "frames for a " << thisPeer->dynamicDelayFrames << "frames delayed message\n";
			//		// Find the first incorrect frame
			//		thisPeer->Rollback_Restore();
			//		int framesToResimulate = static_cast<int>(thisPeer->predictedRemoteStatuses.size() + thisPeer->dynamicDelayFrames);
			//		// Resimulate up to now again
			//		for (int i = 0; i < framesToResimulate; i++)
			//		{
			//			// do as many update cycles as needed to catch up/resimulate up to the current frame
			//			ResimulateFrame();
			//			playerTwo->ResimulateFrame();
			//		}
			//		// Clear the predictions
			//		thisPeer->predictedRemoteStatuses.clear();
			//	}
			//	else
			//	{
			//		ExecuteInput();
			//		playerTwo->ExecuteRemoteInput();
			//	}
			//}

		}
		else if (GetNetworkTechnique() == NetworkTechnique::None)
		{
			// Send our state
			if (thisPeer->localInputThisFrame) thisPeer->SendPlayerStatus(thisPeer->localPlayerStatus);
			// Listen for new messages
			UpdateNetworkState();
		}
	}

	// Check local damage and life status
	if (receivedDamage)
	{
		PushPlayer(sf::Vector2f(static_cast<float>(smallPushDistance), 0));
		playerState = PlayerState::Hurt;
		moveState = MoveState::Idle;
		attackState = AttackState::None;
	}
	else if (hitGuardBox)
	{
		PushPlayer(sf::Vector2f(static_cast<float>(smallPushDistance), 0));
		moveState = MoveState::Idle;
		attackState = AttackState::Defend;
	}
	else
	{
		if (currentHealthPoints <= 0)
		{
			playerState = PlayerState::Dead;
			moveState = MoveState::Idle;
			attackState = AttackState::None;

			shouldAcceptInput = false;
		}
		else playerState = PlayerState::Alive;
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
		thisPeer->ResetLocalPlayerStatus();

		// If we are online and this is the local player
		if (networkAuthority == NetworkAuthority::Local)
		{
			// Send ping request for roundtrip time
			if (input->IsKeyDown(sf::Keyboard::P))
			{
				input->SetKeyUp(sf::Keyboard::P);
				thisPeer->PingRequest();
			}
			if (input->IsKeyDown(sf::Keyboard::O))
			{
				input->SetKeyUp(sf::Keyboard::O);
				thisPeer->SyncStateRequest();
			}

		}

		// If we are using delay, check this frames' input and apply the input that should be applied this frame
		if (networkAuthority == NetworkAuthority::Local && thisPeer->currentNetworkTechnique == NetworkTechnique::InputDelay)
		{
			// Execute this frame's input ..................
			if (!thisPeer->delayedPlayerStatuses.empty())
			{
				for (int i = 0; i < thisPeer->delayedPlayerStatuses.size(); i++)
				{
					if (thisPeer->delayedPlayerStatuses.at(i).appliedDelay == 0)
					{
						// Defend
						if (thisPeer->delayedPlayerStatuses.at(i).Pressed_S && currentEnergyPoints > 0)
						{
							attackState = AttackState::Defend;
						}
						else if (attackState == AttackState::Defend) attackState = AttackState::None;

						// Attack
						// Special punch
						if (thisPeer->delayedPlayerStatuses.at(i).Pressed_W)
						{
							attackState = AttackState::DragonPunch;
						}
						// Punch
						if (playerID == PlayerID::PlayerOne && thisPeer->delayedPlayerStatuses.at(i).HeavyPunched
							|| playerID == PlayerID::PlayerTwo && thisPeer->delayedPlayerStatuses.at(i).HeavyPunched)
						{
							attackState = AttackState::HeavyPunch;

						}
						else if (thisPeer->delayedPlayerStatuses.at(i).Pressed_Q)
						{
							attackState = AttackState::FastPunch;
						}
						// Kick
						if (playerID == PlayerID::PlayerOne && thisPeer->delayedPlayerStatuses.at(i).HeavyKicked
							|| playerID == PlayerID::PlayerTwo && thisPeer->delayedPlayerStatuses.at(i).HeavyKicked)
						{
							attackState = AttackState::HeavyKick;

						}
						else if (thisPeer->delayedPlayerStatuses.at(i).Pressed_E)
						{
							attackState = AttackState::FastKick;
						}
						//-------------------
						// Movement ---------
						if (thisPeer->delayedPlayerStatuses.at(i).Dashed_A) // Dash 
						{
							dashTimer = dashTime;
							setPosition(getPosition().x - dashDistance, getPosition().y);
							moveState = MoveState::DashL;
						}
						else if (thisPeer->delayedPlayerStatuses.at(i).Dashed_D) // Dash 
						{
							dashTimer = dashTime;
							setPosition(getPosition().x + dashDistance, getPosition().y);
							moveState = MoveState::DashR;
						}
						else if (thisPeer->delayedPlayerStatuses.at(i).Pressed_A) // Left
						{
							setPosition(getPosition().x - moveDistance, getPosition().y);
							moveState = MoveState::Left;

						}
						else if (thisPeer->delayedPlayerStatuses.at(i).Pressed_D) // Right
						{
							setPosition(getPosition().x + moveDistance, getPosition().y);
							moveState = MoveState::Right;
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

					// Get rid of the executed input
						thisPeer->delayedPlayerStatuses.erase(thisPeer->delayedPlayerStatuses.begin() + i);
					}
					else thisPeer->delayedPlayerStatuses.at(i).appliedDelay -= 1;
				}
			}

			// CACHE THIS FRAMES INPUT ...................................
			NetworkPeer::PlayerStatus* newPlayerStatus = new NetworkPeer::PlayerStatus; // Create new player status so we can store it
			// Check timers and counters ----------
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

			if (frameAdvantage < 0)
			{
				shouldAcceptInput = false;
			}

			if (!shouldAcceptInput)
			{
				if (attackState == AttackState::Defend && !input->IsKeyDown(sf::Keyboard::S) || attackState == AttackState::Defend && currentEnergyPoints <= 0)
				{
					if (attackState == AttackState::Defend) attackState = AttackState::None;
					shouldAcceptInput = true;
				}
				else if (attackState == AttackState::Defend && input->IsKeyDown(sf::Keyboard::S))
				{
					newPlayerStatus->Pressed_S = true;
				}
			}

			if (shouldAcceptInput)
			{
				// Cache delayed input
				// Defend
				if (input->IsKeyDown(sf::Keyboard::S) && currentEnergyPoints > 0)
				{
					newPlayerStatus->Pressed_S = true;
				}
				// Special punch
				if (input->IsKeyDown(sf::Keyboard::W))
				{
					input->SetKeyUp(sf::Keyboard::W);
					newPlayerStatus->Pressed_W = true;
				}
				// Punch
				if (playerID == PlayerID::PlayerOne && input->IsKeyDown(sf::Keyboard::Q) && input->IsKeyDown(sf::Keyboard::D) || playerID == PlayerID::PlayerTwo && input->IsKeyDown(sf::Keyboard::Q) && input->IsKeyDown(sf::Keyboard::A))
				{
					input->SetKeyUp(sf::Keyboard::Q);
					input->SetKeyUp(sf::Keyboard::D);
					input->SetKeyUp(sf::Keyboard::A);
					newPlayerStatus->HeavyPunched = true;
				}
				else if (input->IsKeyDown(sf::Keyboard::Q))
				{
					input->SetKeyUp(sf::Keyboard::Q);
					newPlayerStatus->Pressed_Q = true;
				}
				// Kick
				if (playerID == PlayerID::PlayerOne && input->IsKeyDown(sf::Keyboard::E) && input->IsKeyDown(sf::Keyboard::D) || playerID == PlayerID::PlayerTwo && input->IsKeyDown(sf::Keyboard::E) && input->IsKeyDown(sf::Keyboard::A))
				{
					input->SetKeyUp(sf::Keyboard::E);
					input->SetKeyUp(sf::Keyboard::D);
					input->SetKeyUp(sf::Keyboard::A);
					newPlayerStatus->HeavyKicked = true;
				}
				else if (input->IsKeyDown(sf::Keyboard::E))
				{
					input->SetKeyUp(sf::Keyboard::E);
					newPlayerStatus->Pressed_E = true;
				}
				//-------------------
				// Movement ---------
				if (input->IsKeyDown(sf::Keyboard::A) && CanGoLeft) // Left
				{
					if (b_dashTriggerL) // Dash 
					{
						dashTimer = dashTime;
						input->SetKeyUp(sf::Keyboard::A);
						newPlayerStatus->Dashed_A = true;
					}
					newPlayerStatus->Pressed_A = true;
				}
				else if (input->IsKeyDown(sf::Keyboard::D) && CanGoRight) // Right
				{
					if (b_dashTriggerR) // Dash 
					{
						dashTimer = dashTime;
						input->SetKeyUp(sf::Keyboard::D);
						newPlayerStatus->Dashed_D = true;
					}
					newPlayerStatus->Pressed_D = true;
				}
				else
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
				}

			}

			newPlayerStatus->appliedDelay = FRAME_DELAY;
			newPlayerStatus->techniqueUsing = NetworkTechnique::InputDelay;
			thisPeer->delayedPlayerStatuses.push_back(*newPlayerStatus); // Store the input
		}
		else if (networkAuthority == NetworkAuthority::Local && thisPeer->currentNetworkTechnique == NetworkTechnique::Rollback)
		{
			ExecuteInput();

			// CACHE THIS FRAMES INPUT ...................................
			NetworkPeer::PlayerStatus* newPlayerStatus = new NetworkPeer::PlayerStatus; // Create new player status so we can store it
			newPlayerStatus->techniqueUsing  = NetworkTechnique::Rollback;
			// Check timers and counters ----------
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

			if (frameAdvantage < 0)
			{
				shouldAcceptInput = false;
			}

			if (!shouldAcceptInput)
			{
				if (attackState == AttackState::Defend && !input->IsKeyDown(sf::Keyboard::S) || attackState == AttackState::Defend && currentEnergyPoints <= 0)
				{
					if (attackState == AttackState::Defend) attackState = AttackState::None;
					shouldAcceptInput = true;
				}
				else if (attackState == AttackState::Defend && input->IsKeyDown(sf::Keyboard::S))
				{
					newPlayerStatus->Pressed_S = true;
				}
			}

			if (shouldAcceptInput)
			{
				// Cache delayed input
				// Defend
				if (input->IsKeyDown(sf::Keyboard::S) && currentEnergyPoints > 0)
				{
					newPlayerStatus->Pressed_S = true;
				}
				// Special punch
				if (input->IsKeyDown(sf::Keyboard::W))
				{
					input->SetKeyUp(sf::Keyboard::W);
					newPlayerStatus->Pressed_W = true;
				}
				// Punch
				if (playerID == PlayerID::PlayerOne && input->IsKeyDown(sf::Keyboard::Q) && input->IsKeyDown(sf::Keyboard::D) || playerID == PlayerID::PlayerTwo && input->IsKeyDown(sf::Keyboard::Q) && input->IsKeyDown(sf::Keyboard::A))
				{
					input->SetKeyUp(sf::Keyboard::Q);
					input->SetKeyUp(sf::Keyboard::D);
					input->SetKeyUp(sf::Keyboard::A);
					newPlayerStatus->HeavyPunched = true;
				}
				else if (input->IsKeyDown(sf::Keyboard::Q))
				{
					input->SetKeyUp(sf::Keyboard::Q);
					newPlayerStatus->Pressed_Q = true;
				}
				// Kick
				if (playerID == PlayerID::PlayerOne && input->IsKeyDown(sf::Keyboard::E) && input->IsKeyDown(sf::Keyboard::D) || playerID == PlayerID::PlayerTwo && input->IsKeyDown(sf::Keyboard::E) && input->IsKeyDown(sf::Keyboard::A))
				{
					input->SetKeyUp(sf::Keyboard::E);
					input->SetKeyUp(sf::Keyboard::D);
					input->SetKeyUp(sf::Keyboard::A);
					newPlayerStatus->HeavyKicked = true;
				}
				else if (input->IsKeyDown(sf::Keyboard::E))
				{
					input->SetKeyUp(sf::Keyboard::E);
					newPlayerStatus->Pressed_E = true;
				}
				//-------------------
				// Movement ---------
				if (input->IsKeyDown(sf::Keyboard::A) && CanGoLeft) // Left
				{
					if (b_dashTriggerL) // Dash 
					{
						dashTimer = dashTime;
						input->SetKeyUp(sf::Keyboard::A);
						newPlayerStatus->Dashed_A = true;
					}
					newPlayerStatus->Pressed_A = true;
				}
				else if (input->IsKeyDown(sf::Keyboard::D) && CanGoRight) // Right
				{
					if (b_dashTriggerR) // Dash 
					{
						dashTimer = dashTime;
						input->SetKeyUp(sf::Keyboard::D);
						newPlayerStatus->Dashed_D = true;
					}
					newPlayerStatus->Pressed_D = true;
				}
				else
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
				}

			}

			newPlayerStatus->appliedDelay = FRAME_DELAY;

			thisPeer->delayedPlayerStatuses.push_back(*newPlayerStatus); // Store the input

		}
		else // Run update without delay ------------------------
		{
		if (thisPeer->currentNetworkTechnique == NetworkTechnique::None) thisPeer->localPlayerStatus.techniqueUsing = NetworkTechnique::None;
		if(thisPeer->currentNetworkTechnique == NetworkTechnique::DeterministicLockstep) thisPeer->localPlayerStatus.techniqueUsing = NetworkTechnique::DeterministicLockstep;
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

			if (frameAdvantage < 0)
			{
				shouldAcceptInput = false;
			}

			if (!shouldAcceptInput)
			{
				if (attackState == AttackState::Defend && !input->IsKeyDown(sf::Keyboard::S) || attackState == AttackState::Defend && currentEnergyPoints <= 0)
				{
					if (attackState == AttackState::Defend) attackState = AttackState::None;
					shouldAcceptInput = true;
				}
				else
				{
					if (networkAuthority == NetworkAuthority::Local && attackState == AttackState::Defend) thisPeer->Pressed_S(); // Inform remote player we are still guarding
				}
			}

			if (shouldAcceptInput)
			{
				// Defend
				if (input->IsKeyDown(sf::Keyboard::S) && currentEnergyPoints > 0)
				{
					attackState = AttackState::Defend;
					shouldAcceptInput = false;

					if (networkAuthority == NetworkAuthority::Local) thisPeer->Pressed_S();
				}
				else if (attackState == AttackState::Defend) attackState = AttackState::None;

				// Attack
				// Special punch
				if (input->IsKeyDown(sf::Keyboard::W))
				{
					input->SetKeyUp(sf::Keyboard::W); // Lift key so it acts as trigger
					attackState = AttackState::DragonPunch;
					if (networkAuthority == NetworkAuthority::Local) thisPeer->Pressed_W();
				}
				// Punch
				if (playerID == PlayerID::PlayerOne && input->IsKeyDown(sf::Keyboard::Q) && input->IsKeyDown(sf::Keyboard::D) || playerID == PlayerID::PlayerTwo && input->IsKeyDown(sf::Keyboard::Q) && input->IsKeyDown(sf::Keyboard::A))
				{

					input->SetKeyUp(sf::Keyboard::Q); // Lift key so it acts as trigger
					attackState = AttackState::HeavyPunch;
					if (networkAuthority == NetworkAuthority::Local) thisPeer->HeavyPunched();

				}
				else if (input->IsKeyDown(sf::Keyboard::Q))
				{
					input->SetKeyUp(sf::Keyboard::Q); // Lift key so it acts as trigger
					attackState = AttackState::FastPunch;

					if (networkAuthority == NetworkAuthority::Local) thisPeer->Pressed_Q();
				}
				// Kick
				if (playerID == PlayerID::PlayerOne && input->IsKeyDown(sf::Keyboard::E) && input->IsKeyDown(sf::Keyboard::D) || playerID == PlayerID::PlayerTwo && input->IsKeyDown(sf::Keyboard::E) && input->IsKeyDown(sf::Keyboard::A))
				{

					input->SetKeyUp(sf::Keyboard::E); // Lift key so it acts as trigger
					attackState = AttackState::HeavyKick;
					if (networkAuthority == NetworkAuthority::Local) thisPeer->HeavyKicked();

				}
				else if (input->IsKeyDown(sf::Keyboard::E))
				{

					input->SetKeyUp(sf::Keyboard::E); // Lift key so it acts as trigger
					attackState = AttackState::FastKick;
					if (networkAuthority == NetworkAuthority::Local) thisPeer->Pressed_E();

				}

				//-------------------
				// Movement ---------
				if (input->IsKeyDown(sf::Keyboard::A) && CanGoLeft) // Left
				{
					if (b_dashTriggerL) // Dash 
					{
						input->SetKeyUp(sf::Keyboard::A);
						dashTimer = dashTime;
						setPosition(getPosition().x - dashDistance, getPosition().y);
						moveState = MoveState::DashL;
						if (networkAuthority == NetworkAuthority::Local) thisPeer->Dashed_A();
					}
					else // Walk
					{
						setPosition(getPosition().x - moveDistance, getPosition().y);
						moveState = MoveState::Left;
						if (networkAuthority == NetworkAuthority::Local) thisPeer->Pressed_A();
					}

				}
				else if (input->IsKeyDown(sf::Keyboard::D) && CanGoRight) // Right
				{
					if (b_dashTriggerR) // Dash 
					{
						input->SetKeyUp(sf::Keyboard::D);
						dashTimer = dashTime;
						setPosition(getPosition().x + dashDistance, getPosition().y);
						moveState = MoveState::DashR;
						if (networkAuthority == NetworkAuthority::Local) thisPeer->Dashed_D();
					}
					else // Walk
					{
						setPosition(getPosition().x + moveDistance, getPosition().y);
						moveState = MoveState::Right;
						if (networkAuthority == NetworkAuthority::Local) thisPeer->Pressed_D();
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

		if (frameAdvantage < 0)
		{
			shouldAcceptInput = false;
		}

		if (input->IsKeyDown(sf::Keyboard::Down) && currentEnergyPoints > 0)
		{
			attackState = AttackState::Defend;
			shouldAcceptInput = false;
		}
		else if (attackState == AttackState::Defend) attackState = AttackState::None;

		if (!shouldAcceptInput)
		{
			if (attackState == AttackState::Defend && !input->IsKeyDown(sf::Keyboard::Down) || attackState == AttackState::Defend && currentEnergyPoints <= 0)
			{
				if (attackState == AttackState::Defend) attackState = AttackState::None;
				shouldAcceptInput = true;
			}
			else return;
		}

		// Attack
		if (input->IsKeyDown(sf::Keyboard::Up))
		{
			input->SetKeyUp(sf::Keyboard::Up); // Lift key so it acts as trigger
			attackState = AttackState::DragonPunch;
		}
		if (input->IsKeyDown(sf::Keyboard::Numpad1) && input->IsKeyDown(sf::Keyboard::Left))
		{
			input->SetKeyUp(sf::Keyboard::Numpad1); // Lift key so it acts as trigger
			attackState = AttackState::HeavyPunch;

		}
		else if (input->IsKeyDown(sf::Keyboard::Numpad1))
		{
			input->SetKeyUp(sf::Keyboard::Numpad1); // Lift key so it acts as trigger
			attackState = AttackState::FastPunch;
		}
		// Kick
		if (input->IsKeyDown(sf::Keyboard::Numpad2) && input->IsKeyDown(sf::Keyboard::Left))
		{
			input->SetKeyUp(sf::Keyboard::Numpad2); // Lift key so it acts as trigger
			attackState = AttackState::HeavyKick;

		}
		else if (input->IsKeyDown(sf::Keyboard::Numpad2))
		{
			input->SetKeyUp(sf::Keyboard::Numpad2); // Lift key so it acts as trigger
			attackState = AttackState::FastKick;

		}

		//-------------------
		// Movement ---------
		if (input->IsKeyDown(sf::Keyboard::Left) && CanGoLeft) // Left
		{
			if (b_dashTriggerL) // Dash 
			{
				dashTimer = dashTime;
				setPosition(getPosition().x - dashDistance, getPosition().y);
				moveState = MoveState::DashL;
			}
			else // Walk
			{
				setPosition(getPosition().x - moveDistance, getPosition().y);
				moveState = MoveState::Left;
			}

		}
		else if (input->IsKeyDown(sf::Keyboard::Right) && CanGoRight) // Right
		{
			if (b_dashTriggerR) // Dash 
			{
				dashTimer = dashTime;
				setPosition(getPosition().x + dashDistance, getPosition().y);
				moveState = MoveState::DashR;
			}
			else // Walk
			{
				setPosition(getPosition().x + moveDistance, getPosition().y);
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
		if (thisPeer->currentNetworkTechnique == NetworkTechnique::InputDelay)
		{
			// Execute this frame's input .................. if delay = 0 then it must be executed this frame
			if (!thisPeer->remoteDelayedPlayerStatuses.empty())
			{
				for (int i = 0; i < thisPeer->remoteDelayedPlayerStatuses.size(); i++)
				{
					// When using dynamic delay, some inputs may go out of scope, just rid of them
					if (thisPeer->remoteDelayedPlayerStatuses.at(i).appliedDelay < 0)
					{
						thisPeer->remoteDelayedPlayerStatuses.erase(thisPeer->remoteDelayedPlayerStatuses.begin() + i);
					}
					else
						// Applied the remote delayed input if we reached the frame it should (0)
						if (thisPeer->remoteDelayedPlayerStatuses.at(i).appliedDelay == 0)
						{

							// Defend
							if (thisPeer->remoteDelayedPlayerStatuses.at(i).Pressed_S && currentEnergyPoints > 0)
							{
								attackState = AttackState::Defend;
							}
							else if (attackState == AttackState::Defend) attackState = AttackState::None;

							if (thisPeer->remoteDelayedPlayerStatuses.at(i).Pressed_W)
							{
								attackState = AttackState::DragonPunch;
							}
							// Punch
							if (thisPeer->remoteDelayedPlayerStatuses.at(i).HeavyPunched)
							{
								attackState = AttackState::HeavyPunch;
							}
							else if (thisPeer->remoteDelayedPlayerStatuses.at(i).Pressed_Q)
							{
								attackState = AttackState::FastPunch;
							}
							// Kick
							if (thisPeer->remoteDelayedPlayerStatuses.at(i).HeavyKicked)
							{
								attackState = AttackState::HeavyKick;
							}
							else if (thisPeer->remoteDelayedPlayerStatuses.at(i).Pressed_E)
							{
								attackState = AttackState::FastKick;
							}

							//-------------------
							// Movement ---------
							if (thisPeer->remoteDelayedPlayerStatuses.at(i).Dashed_A) // Dash 
							{
								setPosition(getPosition().x - dashDistance, getPosition().y);
								moveState = MoveState::DashL;
							}
							else if (thisPeer->remoteDelayedPlayerStatuses.at(i).Pressed_A) // Left
							{
								// Walk
								setPosition(getPosition().x - moveDistance, getPosition().y);
								moveState = MoveState::Left;
							}
							else if (thisPeer->remoteDelayedPlayerStatuses.at(i).Dashed_D) // Dash 
							{
								setPosition(getPosition().x + dashDistance, getPosition().y);
								moveState = MoveState::DashR;
							}
							else if (thisPeer->remoteDelayedPlayerStatuses.at(i).Pressed_D) // Right
							{
								// Walk
								setPosition(getPosition().x + moveDistance, getPosition().y);
								moveState = MoveState::Right;
							}
							else // idle
							{
								moveState = MoveState::Idle;

							}

							thisPeer->remoteDelayedPlayerStatuses.erase(thisPeer->remoteDelayedPlayerStatuses.begin() + i);
						}
						else// Reduce the delay by one frame for all delayed statuses
							if (thisPeer->remoteDelayedPlayerStatuses.at(i).appliedDelay > 0)
							{
								thisPeer->remoteDelayedPlayerStatuses.at(i).appliedDelay -= 1;
							}

				}
			}
		}
		else if (thisPeer->currentNetworkTechnique == NetworkTechnique::Rollback)
		{
			ExecuteRemoteInput();
		}
		else if (thisPeer->currentNetworkTechnique == NetworkTechnique::None || thisPeer->currentNetworkTechnique == NetworkTechnique::DeterministicLockstep)
		{
			// Defend
			if (thisPeer->remotePlayerStatus.Pressed_S && currentEnergyPoints > 0)
			{
				attackState = AttackState::Defend;
				shouldAcceptInput = false;
			}
			else if (attackState == AttackState::Defend) attackState = AttackState::None;

			if (thisPeer->remotePlayerStatus.Pressed_W)
			{
				attackState = AttackState::DragonPunch;
			}
			// Punch
			if (thisPeer->remotePlayerStatus.HeavyPunched)
			{
				attackState = AttackState::HeavyPunch;
			}
			else if (thisPeer->remotePlayerStatus.Pressed_Q)
			{
				attackState = AttackState::FastPunch;
			}
			// Kick
			if (thisPeer->remotePlayerStatus.HeavyKicked)
			{
				attackState = AttackState::HeavyKick;
			}
			else if (thisPeer->remotePlayerStatus.Pressed_E)
			{
				attackState = AttackState::FastKick;
			}

			//-------------------
			// Movement ---------
			if (thisPeer->remotePlayerStatus.Dashed_A) // Dash 
			{
				setPosition(getPosition().x - dashDistance, getPosition().y);
				moveState = MoveState::DashL;
			}
			else if (thisPeer->remotePlayerStatus.Pressed_A) // Left
			{
				// Walk
				setPosition(getPosition().x - moveDistance, getPosition().y);
				moveState = MoveState::Left;
			}
			else if (thisPeer->remotePlayerStatus.Dashed_D) // Dash 
			{
				setPosition(getPosition().x + dashDistance, getPosition().y);
				moveState = MoveState::DashR;
			}
			else if (thisPeer->remotePlayerStatus.Pressed_D) // Right
			{
				// Walk
				setPosition(getPosition().x + moveDistance, getPosition().y);
				moveState = MoveState::Right;
			}
			else // idle
			{
				moveState = MoveState::Idle;

			}
		}

		thisPeer->ResetRemotePlayerStatus();
	}
}

void PlayerCharacter::ExecuteInput()
{
	// Execute this frame's input ..................
	if (!thisPeer->delayedPlayerStatuses.empty())
	{
		for (int i = 0; i < thisPeer->delayedPlayerStatuses.size(); i++)
		{
			if (thisPeer->delayedPlayerStatuses.at(i).appliedDelay == 0)
			{
				// Defend
				if (thisPeer->delayedPlayerStatuses.at(i).Pressed_S && currentEnergyPoints > 0)
				{
					attackState = AttackState::Defend;
				}
				else if (attackState == AttackState::Defend) attackState = AttackState::None;

				// Attack
				// Special punch
				if (thisPeer->delayedPlayerStatuses.at(i).Pressed_W)
				{
					attackState = AttackState::DragonPunch;
				}
				// Punch
				if (playerID == PlayerID::PlayerOne && thisPeer->delayedPlayerStatuses.at(i).HeavyPunched
					|| playerID == PlayerID::PlayerTwo && thisPeer->delayedPlayerStatuses.at(i).HeavyPunched)
				{
					attackState = AttackState::HeavyPunch;

				}
				else if (thisPeer->delayedPlayerStatuses.at(i).Pressed_Q)
				{
					attackState = AttackState::FastPunch;
				}
				// Kick
				if (playerID == PlayerID::PlayerOne && thisPeer->delayedPlayerStatuses.at(i).HeavyKicked
					|| playerID == PlayerID::PlayerTwo && thisPeer->delayedPlayerStatuses.at(i).HeavyKicked)
				{
					attackState = AttackState::HeavyKick;

				}
				else if (thisPeer->delayedPlayerStatuses.at(i).Pressed_E)
				{
					attackState = AttackState::FastKick;
				}
				//-------------------
				// Movement ---------
				if (thisPeer->delayedPlayerStatuses.at(i).Dashed_A) // Dash 
				{
					dashTimer = dashTime;
					setPosition(getPosition().x - dashDistance, getPosition().y);
					moveState = MoveState::DashL;
				}
				else if (thisPeer->delayedPlayerStatuses.at(i).Dashed_D) // Dash 
				{
					dashTimer = dashTime;
					setPosition(getPosition().x + dashDistance, getPosition().y);
					moveState = MoveState::DashR;
				}
				else if (thisPeer->delayedPlayerStatuses.at(i).Pressed_A) // Left
				{
					setPosition(getPosition().x - moveDistance, getPosition().y);
					moveState = MoveState::Left;

				}
				else if (thisPeer->delayedPlayerStatuses.at(i).Pressed_D) // Right
				{
					setPosition(getPosition().x + moveDistance, getPosition().y);
					moveState = MoveState::Right;
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

			// Get rid of the executed input
				thisPeer->delayedPlayerStatuses.erase(thisPeer->delayedPlayerStatuses.begin() + i);
			}
			else thisPeer->delayedPlayerStatuses.at(i).appliedDelay -= 1;
		}
	}



}

void PlayerCharacter::ExecuteRemoteInput()
{
	// Execute this frame's input .................. if delay = 0 then it must be executed this frame
	if (!thisPeer->remoteDelayedPlayerStatuses.empty())
	{
		for (int i = 0; i < thisPeer->remoteDelayedPlayerStatuses.size(); i++)
		{
			// When using dynamic delay, some inputs may go out of scope, just rid of them
			if (thisPeer->remoteDelayedPlayerStatuses.at(i).appliedDelay < 0)
			{
				thisPeer->remoteDelayedPlayerStatuses.erase(thisPeer->remoteDelayedPlayerStatuses.begin() + i);
			}
			else
				// Applied the remote delayed input if we reached the frame it should (0)
				if (thisPeer->remoteDelayedPlayerStatuses.at(i).appliedDelay == 0)
				{

					// Defend
					if (thisPeer->remoteDelayedPlayerStatuses.at(i).Pressed_S && currentEnergyPoints > 0)
					{
						attackState = AttackState::Defend;
					}
					else if (attackState == AttackState::Defend) attackState = AttackState::None;

					if (thisPeer->remoteDelayedPlayerStatuses.at(i).Pressed_W)
					{
						attackState = AttackState::DragonPunch;
					}
					// Punch
					if (thisPeer->remoteDelayedPlayerStatuses.at(i).HeavyPunched)
					{
						attackState = AttackState::HeavyPunch;
					}
					else if (thisPeer->remoteDelayedPlayerStatuses.at(i).Pressed_Q)
					{
						attackState = AttackState::FastPunch;
					}
					// Kick
					if (thisPeer->remoteDelayedPlayerStatuses.at(i).HeavyKicked)
					{
						attackState = AttackState::HeavyKick;
					}
					else if (thisPeer->remoteDelayedPlayerStatuses.at(i).Pressed_E)
					{
						attackState = AttackState::FastKick;
					}

					//-------------------
					// Movement ---------
					if (thisPeer->remoteDelayedPlayerStatuses.at(i).Dashed_A) // Dash 
					{
						setPosition(getPosition().x - dashDistance, getPosition().y);
						moveState = MoveState::DashL;
					}
					else if (thisPeer->remoteDelayedPlayerStatuses.at(i).Pressed_A) // Left
					{
						// Walk
						setPosition(getPosition().x - moveDistance, getPosition().y);
						moveState = MoveState::Left;
					}
					else if (thisPeer->remoteDelayedPlayerStatuses.at(i).Dashed_D) // Dash 
					{
						setPosition(getPosition().x + dashDistance, getPosition().y);
						moveState = MoveState::DashR;
					}
					else if (thisPeer->remoteDelayedPlayerStatuses.at(i).Pressed_D) // Right
					{
						// Walk
						setPosition(getPosition().x + moveDistance, getPosition().y);
						moveState = MoveState::Right;
					}
					else // idle
					{
						moveState = MoveState::Idle;

					}

					thisPeer->remoteDelayedPlayerStatuses.erase(thisPeer->remoteDelayedPlayerStatuses.begin() + i);
				}
				else// Reduce the delay by one frame for all delayed statuses
					if (thisPeer->remoteDelayedPlayerStatuses.at(i).appliedDelay > 0)
					{
						thisPeer->remoteDelayedPlayerStatuses.at(i).appliedDelay -= 1;
					}

		}
	}
}

void PlayerCharacter::ResimulateFrame()
{
	//Check and apply the inputs
	if (networkAuthority == NetworkAuthority::Local)
	{
		ExecuteInput();
	}
	else if (networkAuthority == NetworkAuthority::Remote)
	{
		ExecuteRemoteInput();
	}

	// Update our info on the remote player state
	if (networkAuthority == NetworkAuthority::Remote)
	{
		// Pass the info on local image of the remote player state
		thisPeer->remoteHP = currentHealthPoints;
		thisPeer->remotePosX = static_cast<int>(getPosition().x);

	}
	else
	{
		thisPeer->localHP = currentHealthPoints;
		thisPeer->localPosX = static_cast<int>(getPosition().x);
	}

	// Check local damage and life status
	if (receivedDamage)
	{
		PushPlayer(sf::Vector2f(static_cast<float>(smallPushDistance), 0));
		playerState = PlayerState::Hurt;
		moveState = MoveState::Idle;
		attackState = AttackState::None;
	}
	else if (hitGuardBox)
	{
		PushPlayer(sf::Vector2f(static_cast<float>(smallPushDistance), 0));
		moveState = MoveState::Idle;
		attackState = AttackState::Defend;
	}
	else
	{
		if (currentHealthPoints <= 0)
		{
			playerState = PlayerState::Dead;
			moveState = MoveState::Idle;
			attackState = AttackState::None;

			shouldAcceptInput = false;
		}
		else playerState = PlayerState::Alive;
	}
}

void PlayerCharacter::HandleAnimation(float dt)
{
	// Remember the animation class of the flip state of the sprite
	if (flipped)
	{
		currentAnim->SetFlipped(true);
	}

	// Manage advantage frames
	if (frameAdvantage != 0 && currentAnim->GetCurrentFrame().GetFrameType() == AnimationFrameType::Recovery
		|| frameAdvantage < 0 && currentAnim->IsAnimationCompleted())
	{
		if (receivedGuardBox && currentEnergyPoints <= 0)
		{
			currentAnim = &anim_hurt;
			setTextureRect(currentAnim->GetCurrentFrame().GetRect()); // Set the part of the sprite sheet to draw
			setFillColor(sf::Color(getFillColor().r, getFillColor().g, getFillColor().b, 250)); // Set the color information of the rect to fill it
			animState = currentAnim->GetCurrentFrame().GetFrameType(); // Set the animation state (startup, active, recovery...)
			currentAnim->Animate(dt); // Set to advance frames
		}
		else if (receivedGuardBox)
		{
			currentAnim = &anim_defend;
			setTextureRect(currentAnim->GetCurrentFrame().GetRect()); // Set the part of the sprite sheet to draw
			setFillColor(sf::Color(getFillColor().r, getFillColor().g, getFillColor().b, 250)); // Set the color information of the rect to fill it
			animState = currentAnim->GetCurrentFrame().GetFrameType(); // Set the animation state (startup, active, recovery...)
			currentAnim->Animate(dt); // Set to advance frames
		}

		// Update frame advantage
		if (frameAdvantage > 0) frameAdvantage -= 1;
		if (frameAdvantage < 0) frameAdvantage += 1;

		return;
	}

	// ATTACK --------------------------------------------------------
	// Assign correct attack animation
	switch (attackState)
	{
	case AttackState::None:
		anim_fastkick.ResetAnimation();
		anim_fastPunch.ResetAnimation();
		anim_heavyKick.ResetAnimation();
		anim_heavyPunch.ResetAnimation();
		anim_dragonPunch.ResetAnimation();

		break;
	case AttackState::FastPunch:
		currentAnim = &anim_fastPunch;
		break;
	case AttackState::HeavyPunch:
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

	default:
		break;
	}

	// Go back to attack none if the animation is finished
	if (attackState != AttackState::None && attackState != AttackState::Defend)
	{
		if (currentAnim->IsAnimationCompleted())
		{
			attackState = AttackState::None;
		}

		// Check if we are moving to cancel the attack
		if (currentAnim->GetCurrentFrame().GetFrameType() == AnimationFrameType::Recovery && moveState == MoveState::Left
			|| currentAnim->GetCurrentFrame().GetFrameType() == AnimationFrameType::Recovery && moveState == MoveState::Right)
		{
			attackState = AttackState::None;
		}
	}

	// MOVE - Calculate the move state if we are not attacking
	if (attackState == AttackState::None)
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

	// Check player health state
	switch (playerState)
	{
	case PlayerState::Alive:
		anim_hurt.ResetAnimation();
		break;
	case PlayerState::Dead:
		currentAnim = &anim_die;
		break;
	case PlayerState::Hurt:
		currentAnim = &anim_hurt;
		break;
	default:
		break;
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

		// Check for inflict damage collision --> PLUS ADVANTAGE
		if (collResponse->s1CollType == CollisionBox::ColliderType::HitBox && collResponse->s2CollType == CollisionBox::ColliderType::HurtBox)
		{

			inflictedDamage = true;
		}

		// Check hitting guard box --> MINUS ADVANTAGE
		if (collResponse->s1CollType == CollisionBox::ColliderType::HitBox && collResponse->s2CollType == CollisionBox::ColliderType::GuardBox)
		{
			if (collResponse->s1anim->GetID() == anim_fastPunch.GetID() || collResponse->s1anim->GetID() == anim_fastkick.GetID())
			{
				frameAdvantage = -10;
			}
			if (collResponse->s1anim->GetID() == anim_heavyPunch.GetID() || collResponse->s1anim->GetID() == anim_heavyKick.GetID())
			{
				frameAdvantage = -15;
			}
			if (collResponse->s1anim->GetID() == anim_dragonPunch.GetID())
			{
				frameAdvantage = -35;
			}
		}

		// Check for suffering damage
		if (collResponse->s1CollType == CollisionBox::ColliderType::HurtBox && collResponse->s2CollType == CollisionBox::ColliderType::HitBox)
		{
			if (!receivedDamage)
			{
				// Modify damage receive behaviour based on type of attack
				int modifier = 0;
				if (collResponse->s2anim->GetID() == anim_fastPunch.GetID() || collResponse->s2anim->GetID() == anim_fastkick.GetID())
				{
					modifier = 1;
				}
				if (collResponse->s2anim->GetID() == anim_heavyPunch.GetID() || collResponse->s2anim->GetID() == anim_heavyKick.GetID())
				{
					modifier = 3;
				}
				if (collResponse->s2anim->GetID() == anim_dragonPunch.GetID())
				{
					modifier = 4;
				}

				currentHealthPoints -= 10 * modifier;

				if (currentHealthPoints <= 0) currentHealthPoints = 0;

			}
			receivedDamage = true;
		}

		if (collResponse->s1CollType == CollisionBox::ColliderType::GuardBox && collResponse->s2CollType == CollisionBox::ColliderType::HitBox)
		{
			if (!receivedGuardBox)
			{

				// Modify energy damage receive behaviour based on type of attack
				int modifier = 0;
				if (collResponse->s2anim->GetID() == anim_fastPunch.GetID() || collResponse->s2anim->GetID() == anim_fastkick.GetID())
				{
					modifier = 1;
				}
				if (collResponse->s2anim->GetID() == anim_heavyPunch.GetID() || collResponse->s2anim->GetID() == anim_heavyKick.GetID())
				{
					modifier = 2;
				}
				if (collResponse->s2anim->GetID() == anim_dragonPunch.GetID())
				{
					modifier = 5;
				}
				currentEnergyPoints -= 10 * modifier;
				if (currentEnergyPoints <= 0)
				{
					receivedDamage = true; // so we dont get hit through the broken guard
					frameAdvantage -= 50;
					currentEnergyPoints = 0;
					attackState = AttackState::None;
					moveState = MoveState::Idle;
				}
				else frameAdvantage -= 2 * modifier;
				receivedGuardBox = true;
				hitGuardBox = true;
			}
		}

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

		// Check hitting guard box
		if (collResponse->s2CollType == CollisionBox::ColliderType::GuardBox && collResponse->s1CollType == CollisionBox::ColliderType::HitBox)
		{
			if (collResponse->s2anim->GetID() == anim_fastPunch.GetID() || collResponse->s2anim->GetID() == anim_fastkick.GetID())
			{
				frameAdvantage = -10;
			}
			if (collResponse->s2anim->GetID() == anim_heavyPunch.GetID() || collResponse->s2anim->GetID() == anim_heavyKick.GetID())
			{
				frameAdvantage = -15;
			}
			if (collResponse->s2anim->GetID() == anim_dragonPunch.GetID())
			{
				frameAdvantage = -35;
			}
		}

		// Check for suffering damage
		if (collResponse->s2CollType == CollisionBox::ColliderType::HurtBox && collResponse->s1CollType == CollisionBox::ColliderType::HitBox)
		{
			if (!receivedDamage)
			{
				// Modify damage receive behaviour based on type of attack
				int modifier = 0;
				if (collResponse->s1anim->GetID() == anim_fastPunch.GetID() || collResponse->s1anim->GetID() == anim_fastkick.GetID())
				{
					modifier = 1;
				}
				if (collResponse->s1anim->GetID() == anim_heavyPunch.GetID() || collResponse->s1anim->GetID() == anim_heavyKick.GetID())
				{
					modifier = 3;
				}
				if (collResponse->s1anim->GetID() == anim_dragonPunch.GetID())
				{
					modifier = 4;
				}

				currentHealthPoints -= 10 * modifier;
				if (currentHealthPoints <= 0) currentHealthPoints = 0;
			}
			receivedDamage = true;
		}
		else receivedDamage = false;

		if (collResponse->s2CollType == CollisionBox::ColliderType::GuardBox && collResponse->s1CollType == CollisionBox::ColliderType::HitBox)
		{
			if (!receivedGuardBox)
			{
				// Modify energy damage receive behaviour based on type of attack
				int modifier = 0;
				if (collResponse->s1anim->GetID() == anim_fastPunch.GetID() || collResponse->s1anim->GetID() == anim_fastkick.GetID())
				{
					modifier = 1;
				}
				if (collResponse->s1anim->GetID() == anim_heavyPunch.GetID() || collResponse->s1anim->GetID() == anim_heavyKick.GetID())
				{
					modifier = 2;
				}
				if (collResponse->s1anim->GetID() == anim_dragonPunch.GetID())
				{
					modifier = 5;
				}
				currentEnergyPoints -= 10 * modifier;
				if (currentEnergyPoints <= 0)
				{
					receivedDamage = true; // so we dont get hit through the broken guard
					frameAdvantage -= 50;
					currentEnergyPoints = 0;
					attackState = AttackState::None;
					moveState = MoveState::Idle;
				}
				else frameAdvantage -= 2 * modifier;
				receivedGuardBox = true;
				hitGuardBox = true;
			}
		}


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
	if (receivedGuardBox) receivedGuardBox = false;
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
	anim_idle.SetID(0);
	// Walk -----
	anim_walkFWD.AddFrame(sf::IntRect(0, 55, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_walkFWD.AddFrame(sf::IntRect(78, 55, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_walkFWD.AddFrame(sf::IntRect(156, 55, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_walkFWD.AddFrame(sf::IntRect(234, 55, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_walkFWD.AddFrame(sf::IntRect(312, 55, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_walkFWD.AddFrame(sf::IntRect(390, 55, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_walkFWD.AddFrame(sf::IntRect(0, 110, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_walkFWD.SetFrameSpeed(0.1f);
	anim_walkFWD.SetID(1);

	// Defend ---
	CollisionBox* GuardColl = new CollisionBox(CollisionBox::ColliderType::GuardBox, bodycallPos, bodycallSize, bodyCollOffset);
	anim_defend.AddFrame(sf::IntRect(0, 165, 78, 55), AnimationFrameType::Active, *GuardColl);
	//anim_defend.AddFrame(sf::IntRect(0, 165, 78, 55), AnimationFrameType::Recovery, *GuardColl);
	anim_defend.SetLooping(false);
	anim_defend.SetID(2);

	// Dash FWD and BKW ---
	anim_dashFWD.AddFrame(sf::IntRect(156, 110, 78, 55), AnimationFrameType::StartUp, *bodyColl);
	anim_dashFWD.AddFrame(sf::IntRect(234, 110, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_dashFWD.AddFrame(sf::IntRect(312, 110, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_dashFWD.AddFrame(sf::IntRect(312, 110, 78, 55), AnimationFrameType::Recovery, *bodyColl);
	anim_dashFWD.SetLooping(false);
	anim_dashFWD.SetFrameSpeed(0.1f);
	anim_dashFWD.SetID(3);

	anim_dashBKW.AddFrame(sf::IntRect(390, 110, 78, 55), AnimationFrameType::StartUp, *bodyColl);
	anim_dashBKW.AddFrame(sf::IntRect(312, 110, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_dashBKW.AddFrame(sf::IntRect(312, 110, 78, 55), AnimationFrameType::Idle, *bodyColl);
	anim_dashBKW.AddFrame(sf::IntRect(312, 110, 78, 55), AnimationFrameType::Recovery, *bodyColl);
	anim_dashBKW.SetLooping(false);
	anim_dashBKW.SetFrameSpeed(0.1f);
	anim_dashBKW.SetID(4);

	// Hurt anim -----
	anim_hurt.AddFrame(sf::IntRect(78, 165, 78, 55), AnimationFrameType::Active, *bodyColl);
	anim_hurt.AddFrame(sf::IntRect(156, 165, 78, 55), AnimationFrameType::Active, *bodyColl);
	anim_hurt.AddFrame(sf::IntRect(234, 165, 78, 55), AnimationFrameType::Active, *bodyColl);
	anim_hurt.AddFrame(sf::IntRect(234, 165, 78, 55), AnimationFrameType::Recovery, *bodyColl);
	anim_hurt.AddFrame(sf::IntRect(234, 165, 78, 55), AnimationFrameType::Recovery, *bodyColl);
	anim_hurt.SetLooping(false);
	anim_hurt.SetFrameSpeed(0.1f);
	anim_hurt.SetID(5);

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
	anim_fastPunch.AddFrame(sf::IntRect(78, 220, 78, 55), AnimationFrameType::Recovery, *bodyColl);
	anim_fastPunch.SetFrameSpeed(0.1f);
	anim_fastPunch.SetLooping(false);
	anim_fastPunch.SetID(6);

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
	anim_heavyPunch.AddFrame(sf::IntRect(312, 220, 78, 55), AnimationFrameType::Recovery, *bodyColl);
	anim_heavyPunch.AddFrame(sf::IntRect(312, 220, 78, 55), AnimationFrameType::Recovery, *bodyColl);
	anim_heavyPunch.AddFrame(sf::IntRect(312, 220, 78, 55), AnimationFrameType::Recovery, *bodyColl);
	anim_heavyPunch.SetFrameSpeed(0.1f);
	anim_heavyPunch.SetLooping(false);
	anim_heavyPunch.SetID(7);

	// fast kick
	sf::Vector2f kickCollOffset = sf::Vector2f(static_cast <float>(15 * PIXEL_SCALE_FACTOR), static_cast <float>(15 * PIXEL_SCALE_FACTOR));
	sf::Vector2f kickCollPos = getPosition() + kickCollOffset;
	sf::Vector2f kickCollSize = sf::Vector2f(static_cast <float>(15 * PIXEL_SCALE_FACTOR), static_cast <float>(10 * PIXEL_SCALE_FACTOR));
	CollisionBox* kickColl = new CollisionBox(CollisionBox::ColliderType::HitBox, kickCollPos, kickCollSize, kickCollOffset);

	std::vector<CollisionBox*> kickCollVector;
	kickCollVector.push_back(bodyColl);
	kickCollVector.push_back(kickColl);

	anim_fastkick.AddFrame(sf::IntRect(0, 275, 78, 55), AnimationFrameType::StartUp, *bodyColl);
	anim_fastkick.AddFrame(sf::IntRect(78, 275, 78, 55), AnimationFrameType::Active, kickCollVector);
	anim_fastkick.AddFrame(sf::IntRect(78, 275, 78, 55), AnimationFrameType::Active, kickCollVector);
	anim_fastkick.AddFrame(sf::IntRect(156, 275, 78, 55), AnimationFrameType::Recovery, *bodyColl);
	anim_fastkick.AddFrame(sf::IntRect(156, 275, 78, 55), AnimationFrameType::Recovery, *bodyColl);
	anim_fastkick.SetFrameSpeed(0.1f);
	anim_fastkick.SetLooping(false);
	anim_fastkick.SetID(8);

	// Heavy kick
	sf::Vector2f hkickCollOffset = sf::Vector2f(static_cast <float>(20 * PIXEL_SCALE_FACTOR), static_cast <float>(-8 * PIXEL_SCALE_FACTOR));
	sf::Vector2f hkickCollPos = getPosition() + hkickCollOffset;
	sf::Vector2f hkickCollSize = sf::Vector2f(static_cast <float>(18 * PIXEL_SCALE_FACTOR), static_cast <float>(10 * PIXEL_SCALE_FACTOR));
	CollisionBox* hkickColl = new CollisionBox(CollisionBox::ColliderType::HitBox, hkickCollPos, hkickCollSize, hkickCollOffset);

	std::vector<CollisionBox*> hkickCollVector;
	hkickCollVector.push_back(bodyColl);
	hkickCollVector.push_back(hkickColl);

	anim_heavyKick.AddFrame(sf::IntRect(234, 275, 78, 55), AnimationFrameType::StartUp, *bodyColl);
	anim_heavyKick.AddFrame(sf::IntRect(312, 275, 78, 55), AnimationFrameType::StartUp, *bodyColl);
	anim_heavyKick.AddFrame(sf::IntRect(390, 275, 78, 55), AnimationFrameType::StartUp, *bodyColl);
	anim_heavyKick.AddFrame(sf::IntRect(0, 330, 78, 55), AnimationFrameType::Active, hkickCollVector);
	anim_heavyKick.AddFrame(sf::IntRect(0, 330, 78, 55), AnimationFrameType::Active, hkickCollVector);
	anim_heavyKick.AddFrame(sf::IntRect(0, 330, 78, 55), AnimationFrameType::Active, hkickCollVector);
	anim_heavyKick.AddFrame(sf::IntRect(78, 330, 78, 55), AnimationFrameType::Recovery, *bodyColl);
	anim_heavyKick.AddFrame(sf::IntRect(78, 330, 78, 55), AnimationFrameType::Recovery, *bodyColl);
	anim_heavyKick.AddFrame(sf::IntRect(234, 275, 78, 55), AnimationFrameType::Recovery, *bodyColl);
	anim_heavyKick.SetFrameSpeed(0.1f);
	anim_heavyKick.SetLooping(false);
	anim_heavyKick.SetID(9);

	// Dragon punch
	sf::Vector2f  dpunchCollOffset = sf::Vector2f(static_cast <float>(15 * PIXEL_SCALE_FACTOR), static_cast <float>(-10 * PIXEL_SCALE_FACTOR));
	sf::Vector2f dpunchCollPos = getPosition() + dpunchCollOffset;
	sf::Vector2f dpunchCollSize = sf::Vector2f(static_cast <float>(10 * PIXEL_SCALE_FACTOR), static_cast <float>(10 * PIXEL_SCALE_FACTOR));
	CollisionBox* dpunchColl = new CollisionBox(CollisionBox::ColliderType::HitBox, dpunchCollPos, dpunchCollSize, dpunchCollOffset);

	std::vector<CollisionBox*> dpunchCollVector;
	dpunchCollVector.push_back(bodyColl);
	dpunchCollVector.push_back(dpunchColl);

	anim_dragonPunch.AddFrame(sf::IntRect(156, 330, 78, 55), AnimationFrameType::StartUp, *bodyColl);
	anim_dragonPunch.AddFrame(sf::IntRect(234, 330, 78, 55), AnimationFrameType::StartUp, *bodyColl);
	anim_dragonPunch.AddFrame(sf::IntRect(312, 330, 78, 55), AnimationFrameType::Active, dpunchCollVector);
	anim_dragonPunch.AddFrame(sf::IntRect(390, 330, 78, 55), AnimationFrameType::Active, dpunchCollVector);
	anim_dragonPunch.AddFrame(sf::IntRect(0, 385, 78, 55), AnimationFrameType::Active, dpunchCollVector);
	anim_dragonPunch.AddFrame(sf::IntRect(78, 385, 78, 55), AnimationFrameType::Recovery, *bodyColl);
	anim_dragonPunch.AddFrame(sf::IntRect(78, 385, 78, 55), AnimationFrameType::Recovery, *bodyColl);
	anim_dragonPunch.AddFrame(sf::IntRect(78, 385, 78, 55), AnimationFrameType::Recovery, *bodyColl);
	anim_dragonPunch.AddFrame(sf::IntRect(78, 385, 78, 55), AnimationFrameType::Recovery, *bodyColl);
	anim_dragonPunch.SetFrameSpeed(0.07f);
	anim_dragonPunch.SetLooping(false);
	anim_dragonPunch.SetID(10);
	// Die
	anim_die.AddFrame(sf::IntRect(156, 385, 78, 55), AnimationFrameType::Active);
	anim_die.AddFrame(sf::IntRect(234, 385, 78, 55), AnimationFrameType::Active);
	anim_die.AddFrame(sf::IntRect(312, 385, 78, 55), AnimationFrameType::Active);
	anim_die.AddFrame(sf::IntRect(390, 385, 78, 55), AnimationFrameType::Active);
	anim_die.AddFrame(sf::IntRect(0, 440, 78, 55), AnimationFrameType::Active);
	anim_die.SetFrameSpeed(0.1f);
	anim_die.SetLooping(false);
	anim_die.SetID(11);

	// Initialize to idle
	currentAnim = &anim_idle;
}

void PlayerCharacter::PushPlayer(sf::Vector2f distance)
{
	if (flipped)
	{
		setPosition(getPosition() + distance);
	}
	else setPosition(getPosition() - distance);

}
