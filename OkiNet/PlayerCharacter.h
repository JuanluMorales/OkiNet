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
	int currentHealthPoints; //Current health 
	int currentEnergyPoints;
	enum class MoveState { Idle, Left, Right, DashL, DashR };
	bool CanGoLeft; //Allows the player to move left 
	bool CanGoRight;
	enum class AttackState { None, FastPunch, HeavyPunch, FastKick, HeavyKick, DragonPunch, Defend };

	PlayerCharacter();
	~PlayerCharacter();

	void InitCharacter(PlayerID id, sf::Vector2f startPos);

	void InitNetworkedCharacter(PlayerID id, sf::Vector2f startPos, std::shared_ptr<NetworkPeer> peerRef, bool isLocalCharacter);
	SyncState GetSyncState() { return thisPeer->currentSyncState; }
	NetworkTechnique GetNetworkTechnique() { return thisPeer->currentNetworkTechnique; }
	void SetNetworkTechnique(NetworkTechnique nt) { thisPeer->currentNetworkTechnique = nt; }
	bool HasReceivedRemoteUpdateThisFrame() { return thisPeer->receivedRemoteUpdateThisFrame; }
	const int GetCurrentDelayFrames() { return FRAME_DELAY; }


	void Update(float dt, sf::Window* wnd, PlayerCharacter* playerTwo); // Update states and apply transformations, needs a reference to the other local player for rollback purposes
	void UpdateNetworkState();

	void HandleInput(InputManager* input, float dt); // Register input
	void HandleRemotePlayerInput(InputManager* input, float dt); // update the remote player input based on the network state
	void ExecuteInput(); // Updates states and positions based on the input information registered on HandleInput()
	void ExecuteRemoteInput(); // Update local states for the local representation of the remote player

	void ResimulateFrame(); // Resimulate the frame when using rollback

	Animation* GetCurrentAnimation() { return currentAnim; }; // Return the animation currently being played
	std::vector<CollisionBox*> GetCurrentCollision() { return currentAnim->GetCurrentBoxes(); }; // Returns a vector of hitboxes for the frame

	// Collision methods to be publicly available
	void CollisionResponseToPlayer(Collision::CollisionResponse* collResponse); // Response to collision initial contact, it is assumed the local player will always be s1 in the collResponse
	void NoCollisionRegistered(); // Called when no collision was registered to check statuses based on collision such as movement

private:
	
	void HandleAnimation(float dt); // Update animation state
	void SetUpAnimationFrames(); // Encapsulate the frame setup for the animations (animation frames + collision boxes)

	void PushPlayer(sf::Vector2f distance);

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

	const int maxHealthPoints = 100; //Total hit points the player can suffer before dying
	const int maxEnergyPoints = 100;

	bool CharacterSetUp; // Is the character ready for game rendering and updating?

	// Network --------------
	// The peer object contains all the network functionality and is shared with the scene class
	std::shared_ptr<NetworkPeer> thisPeer;

	enum class NetworkAuthority { Offline, Local, Remote };
	NetworkAuthority networkAuthority; // Assuming multiplayer, is this the local or the remote character?

	// input delay
	int remoteFrameWaitCounter = 0; // Tracks the frame we have waited to receive further notice of the remote player for input delay purposes
	int localInputDelayCounter = 0; // Helper to track the current frames we have waited until delay input can be executed
	int FRAME_DELAY = 0; //Tracks the frames to delay the input

	// Rollback counters and timers/helpers
	int rollbackBlockCounter = 0; // When rollbackBlockCounter == RollbackFrames.size(), it means we can no longer keep predicting the remote status and we must lockstep

	bool pingRequestThisFrame = false; // Helper for input to act as button
	bool syncStateRequestThisFrame = false; // Same as ping
};

