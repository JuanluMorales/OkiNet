#pragma once
#include "InputManager.h" // To enable input from this class instead of from the scene class for encapsulation
#include "Animation.h"
#include <vector>

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

	void Update(float dt, sf::Window* wnd); // Update states and apply transformations
	void HandleInput(InputManager* input, float dt); // Register input

	Animation* GetCurrentAnimation() { return currentAnim; }; // Return the animation currently being played
	std::vector<CollisionBox>& GetCurrentCollision() { return currentAnim->GetBoxes(); }; // Returns a vector of hitboxes for the frame

	// Collision methods to be publicly available
	void CollisionResponseToPlayer(Collision::CollisionResponse* collResponse); // Response to collision initial contact, it is assumed the local player will always be s1 in the collResponse
	void NoCollisionRegistered(); // Called when no collision was registered to check statuses based on collision such as movement


private:
	
	void HandleAnimation(float dt); // Update animation state
	void SetUpAnimationFrames(); // Encapsulate the frame setup for the animations (animation frames + collision boxes)
	//void SetUpCollision();

	int moveDistance; // Amount of distance moved while walking
	int dashDistance; // Dash translation factor

	sf::Texture texture; // Contains the graphics information from the sprite sheet to be extracted 
	//List of animations 
	Animation* currentAnim; //Holds current animation and is the one to be updated like currentAnim = &animation;
	Animation anim_idle; //when no input received for the player
	Animation anim_walkFWD; // Right walk
	Animation anim_walkBKW; // Left walk
	Animation anim_dashFWD; // Forward dash
	Animation anim_dashBKW; // Backward dash
	Animation anim_fastPunch;
	Animation anim_defend;

	// Animation related variables
	int hitStun; // Amount to be stunned when receiving a hit
	int blockStun; // Amount to be stunned when receiving a hit while blocking
	int frameAdvantage; // Can be positive or negative depending on the situation. Represents the speed advantage of an attack

	//// List of collision boxes
	//// Body collision
	//CollisionBox* bodyColl;
	sf::Vector2f bodyCollOffset;
	//// Punch boxes
	//CollisionBox* punchColl;

	PlayerID playerID; // Is this the first or second player
	PlayerState playerState; // Alive by default
	MoveState moveState; // Idle by default
	AttackState attackState; 
	AnimationFrameType animState;

	bool shouldAcceptInput;

	bool b_dashTriggerL; // Helps checking if player can/wants to dash
	bool b_dashTriggerR;
	float dashTime; // Time between consecutive presses of the move button to recognise as a dash 
	float dashTimer; // Helper counter

	bool b_fastPunch; // Helper for triggering the animation on/off

	bool grounded; //Sets the player able to jump when true
	bool CanGoLeft; //Allows the player to move left 
	bool CanGoRight;

	int maxHealthPoints; //Total hit points the player can suffer before dying
	int currentHealthPoints; //Current health 

	bool CharacterSetUp; // Is the character ready for game rendering and updating?


};

