#include "PlayerCharacter.h"


PlayerCharacter::PlayerCharacter()
{	
	// Set character defaults
	playerID = PlayerID::PlayerOne;
	playerState = PlayerState::Alive; // Alive by default
	moveState = MoveState::Idle; // Idle by default

	grounded = false; //Sets the player able to jump when true
	CanGoLeft = true; //Allows the player to move left 
	CanGoRight = true; // "   "    "     "		"	right
	CanGoUp = true; //If theres a ceiling, doesnt allow to go up
	maxHealthPoints = 50; //Total hit points the player can suffer before dying
	currentHealthPoints = 50; //Current health 
	MoveSpeed = 100; //Multiplier for the movement speed

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
		texture.loadFromFile("Sprites/Ryu.png");
		setSize(sf::Vector2f(67, 82));
		setScale(PIXEL_SCALE_FACTOR, PIXEL_SCALE_FACTOR);
		setTexture(&texture);
	}
	else
	{
		// Configure player two graphics
		texture.loadFromFile("Sprites/Akuma.png");
		setSize(sf::Vector2f(67, 98));
		setScale(PIXEL_SCALE_FACTOR, PIXEL_SCALE_FACTOR);
		setTexture(&texture);
	}

	setPosition(startPos);
	CharacterSetUp = true;
}

PlayerCharacter::~PlayerCharacter()
{
}

//Manages the movement and animation of the player
void PlayerCharacter::update(float dt, sf::Window* wnd)
{
	/*
	//Apply gravity force, increasing velocity ----------
	velocity.y += gravity * dt;

	//Check if player should be pushed
	if (ShouldBePushed && PushCounter != PushLimit) {
		if (CanGoLeft) {
			setVelocity(sf::Vector2f(-1, 0) * pushForce * dt);
		}
		else if (CanGoRight) {
			setVelocity(sf::Vector2f(1, 0) * pushForce * dt);
		}

		move(velocity);
		PushCounter++;
	}
	else if (PushCounter >= PushLimit && ShouldBePushed) {
		setVelocity(0, 0);
		ShouldBePushed = false;
	}

	//If player should lose input because of hitstun etc	
	if (ShouldLoseInput && InputCounter != InputTimer) {
		InputCounter++;
		return;
	}
	else if (InputCounter >= InputTimer) ShouldLoseInput = false;
	*/

	//// Apply movement 
	//if (input->isKeyDown(sf::Keyboard::D)) { //Right
	//	if (CanGoRight) { //If there is no collision impeding
	//		move(sf::Vector2f(MoveSpeed, 0) * dt);
	//	}

	//}
	//if (input->isKeyDown(sf::Keyboard::A)) { //Left
	//	if (CanGoLeft) {
	//		move(sf::Vector2f(-MoveSpeed, 0) * dt);
	//	}
	//}

}

void PlayerCharacter::handleInput(InputManager* input)
{
	
}
