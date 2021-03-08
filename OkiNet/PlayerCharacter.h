#pragma once
#include "SpriteBase.h" // Sprite graphics
#include "InputManager.h" // To enable input from this class for encapsulation
#include "Animation.h"
#include "Collision.h"

class PlayerCharacter : public SpriteBase
{
public:
	enum class PlayerID { PlayerOne, PlayerTwo };
	enum class PlayerState { Alive, Dead, };
	enum class MoveState { Idle, Left, Right, DashL, DashR };
	enum class AttackState { None, FastPunch, Defend };

	PlayerCharacter();
	void InitCharacter(PlayerID id, sf::Vector2f startPos);
	~PlayerCharacter();

	void update(float dt, sf::Window* wnd); // Update states and apply transformations
	void handleInput(InputManager* input, float dt); // Register input
	void HandleAnimation(float dt);
	int MoveSpeed; //Multiplier for the movement speed

	//List of animations 
	Animation anim_idle; //when no input received for the player
	Animation anim_walkFWD; // Right walk
	Animation anim_walkBKW; // Left walk
	Animation anim_dashFWD; // Forward dash
	Animation anim_dashBKW; // Backward dash
	Animation anim_fastPunch;
	Animation anim_defend;
	

	void collisionResponse(SpriteBase* sp, Collision::CollisionResponse* collResponse);

	// Body collision
	CollisionBox* bodyColl;
	sf::Vector2f bodyCollOffset;

protected:
	sf::Texture texture; // The graphic component of the character

	PlayerID playerID; // Is this the first or second player
	PlayerState playerState; // Alive by default
	MoveState moveState; // Idle by default
	AttackState attackState; 
	AnimationFrameType animState;

	bool shouldAcceptInput;

	float dashDistance;
	bool b_dashTriggerL; // Helps checking if player can/wants to dash
	bool b_dashTriggerR;
	float dashTime; // Time between consecutive presses of the move button to recognise as a dash 
	float dashTimer; // Helper counter

	bool b_fastPunch; // Helper for triggering the animation on/off

	bool grounded; //Sets the player able to jump when true
	bool CanGoLeft; //Allows the player to move left 
	bool CanGoRight; // "   "    "     "		"	right
	bool CanGoUp; //If theres a ceiling, doesnt allow to go up

	int maxHealthPoints; //Total hit points the player can suffer before dying
	int currentHealthPoints; //Current health 

	bool CharacterSetUp; // Is the character ready for game rendering and updating?

	Animation* currentAnim; //Holds current animation and is the one to be updated like currentAnim = &animation;

	void SetUpAnimations(); // Encapsulate the frame setup for the animations
};

