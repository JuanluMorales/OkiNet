#pragma once
#include "InputManager.h" // To enable input from this class instead of from the scene class for encapsulation
#include "Animation.h"
#include <vector>
#include "NetworkPeer.h"

class PlayerCharacter : public SpriteBase
{
public:
	enum class PlayerID { PlayerOne, PlayerTwo };
	enum class PlayerState { Alive, Dead, Hurt };
	float currentHealthPoints; //Current health 
	float currentEnergyPoints;
	enum class MoveState { Idle, Left, Right, DashL, DashR };
	bool CanGoLeft; //Allows the player to move left 
	bool CanGoRight;
	enum class AttackState { None, FastPunch, HeavyPunch, FastKick, HeavyKick, DragonPunch, Defend };

	PlayerCharacter();
	void InitCharacter(PlayerID id, sf::Vector2f startPos);
	void InitNetworkedCharacter(PlayerID id, sf::Vector2f startPos, std::shared_ptr<NetworkPeer> peerRef, bool isLocalCharacter);
	SyncState GetSyncState() { return thisPeer->currentSyncState; }
	NetworkTechnique GetNetworkTechnique() { return thisPeer->currentNetworkTechnique; }
	void SetNetworkTechnique(NetworkTechnique nt) { thisPeer->currentNetworkTechnique = nt; }
	~PlayerCharacter();

	void Update(float dt, sf::Window* wnd); // Update states and apply transformations

	void HandleInput(InputManager* input, float dt); // Register input
	void HandleRemotePlayerInput(InputManager* input, float dt); // update the remote player input based on the network state

	Animation* GetCurrentAnimation() { return currentAnim; }; // Return the animation currently being played
	std::vector<CollisionBox*> GetCurrentCollision() { return currentAnim->GetCurrentBoxes(); }; // Returns a vector of hitboxes for the frame

	// Collision methods to be publicly available
	void CollisionResponseToPlayer(Collision::CollisionResponse* collResponse); // Response to collision initial contact, it is assumed the local player will always be s1 in the collResponse
	void NoCollisionRegistered(); // Called when no collision was registered to check statuses based on collision such as movement

private:
	
	void HandleAnimation(float dt); // Update animation state
	void SetUpAnimationFrames(); // Encapsulate the frame setup for the animations (animation frames + collision boxes)

	void PushPlayer(sf::Vector2f distance, float dt);

	int moveDistance; // Amount of distance moved while walking
	int dashDistance; // Dash translation factor
	int smallPushDistance;

	sf::Texture texture; // Contains the graphics information from the sprite sheet to be extracted 
	//List of animations 
	Animation* currentAnim; //Holds current animation and is the one to be updated like currentAnim = &animation;
	Animation anim_idle; //when no input received for the player
	Animation anim_walkFWD; // Right walk
	Animation anim_walkBKW; // Left walk
	Animation anim_dashFWD; // Forward dash
	Animation anim_dashBKW; // Backward dash
	Animation anim_fastPunch;
	Animation anim_heavyPunch;
	Animation anim_fastkick;
	Animation anim_heavyKick;
	Animation anim_dragonPunch;
	Animation anim_defend;
	Animation anim_hurt;
	Animation anim_die;
	int frameAdvantage = 0; // Depending on the attack, can be positive or negative

	PlayerID playerID; // Is this the first or second player
	PlayerState playerState; // Alive by default
	MoveState moveState; // Idle by default
	AttackState attackState; 
	AnimationFrameType animState;

	bool shouldAcceptInput;

	// Helpers for collision and state updating
	bool receivedDamage;
	bool inflictedDamage;
	bool hitGuardBox;
	bool receivedGuardBox; // has the player received damage while guarding during this frame?

	bool b_dashTriggerL; // Helps checking if player can/wants to dash
	bool b_dashTriggerR;
	float dashTime; // Time between consecutive presses of the move button to recognise as a dash 
	float dashTimer; // Helper counter

	bool b_fastPunch; // Helper for triggering the animation on/off

	bool grounded; //Sets the player able to jump when true


	const float maxHealthPoints = 100; //Total hit points the player can suffer before dying
	const float maxEnergyPoints = 100;

	bool CharacterSetUp; // Is the character ready for game rendering and updating?

	// Network --------------
	// The peer object contains all the network functionality and is shared with the scene class
	std::shared_ptr<NetworkPeer> thisPeer;

	enum class NetworkAuthority { Offline, Local, Remote };
	NetworkAuthority networkAuthority; // Assuming multiplayer, is this the local or the remote character?
};

