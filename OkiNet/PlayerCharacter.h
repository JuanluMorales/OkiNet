#pragma once
#include "SpriteBase.h" // Sprite graphics
#include "InputManager.h" // To enable input from this class for encapsulation
#include "Collision.h"


class PlayerCharacter : public SpriteBase
{
public:
	enum class PlayerID { PlayerOne, PlayerTwo };
	enum class PlayerState { Alive, Dead, };
	enum class MoveState { Idle, Left, Right };

	PlayerCharacter();
	void InitCharacter(PlayerID id, sf::Vector2f startPos);
	~PlayerCharacter();

	void update(float dt, sf::Window* wnd); // Update states and apply transformations
	void handleInput(InputManager* input); // Register input

protected:
	sf::Texture texture; // The graphic component of the character

	PlayerID playerID; // Is this the first or second player
	PlayerState playerState = PlayerState::Alive; // Alive by default
	MoveState moveState = MoveState::Idle; // Idle by default


	bool grounded; //Sets the player able to jump when true
	bool CanGoLeft; //Allows the player to move left 
	bool CanGoRight; // "   "    "     "		"	right
	bool CanGoUp; //If theres a ceiling, doesnt allow to go up

	int maxHealthPoints; //Total hit points the player can suffer before dying
	int currentHealthPoints; //Current health 

	// Input variables
	int MoveSpeed = 400; //Multiplier for the movement speed

	bool CharacterSetUp; // Is the character ready for game rendering and updating?

};

