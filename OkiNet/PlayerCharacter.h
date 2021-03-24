#pragma once
#include "InputManager.h" // To enable input from this class instead of from the scene class for encapsulation
#include "Animation.h"
#include <vector>
#include <net_common_headers.h>

// Holds the IDs to interpret the messages
enum class MsgTypes : uint32_t
{
	PingRequest,
	PingAnswer,
	Move,
	Dash,
	FastPunch
};

// Inherit from the base peer class to override and add functionality
class CustomPeer : public net::Peer<MsgTypes>
{
public:
	CustomPeer(uint16_t port) : net::Peer<MsgTypes>(port)
	{

	}
	void Move()
	{
		net::message<MsgTypes> msg;
		msg.header.id = MsgTypes::Move;
		Send(msg);
		std::cout << "Sent move message.\n";
	}

	// Send a ping request to retrieve the roundtrip time
	void PingRequest()
	{
		net::message<MsgTypes> msg;
		msg.header.id = MsgTypes::PingRequest;
		std::chrono::system_clock::time_point timenow = std::chrono::system_clock::now();
		msg << timenow;
		Send(msg);

	}

protected:
	virtual bool OnClientConnect()
	{
		return true;
	}

	virtual void OnClientDisconnect()
	{
		std::cout << "Peer disconnected.\n";
		peerDisconnected = true;
	}

	virtual void OnMessageReceived(net::message<MsgTypes>& msg)
	{
		switch (msg.header.id)
		{
		case MsgTypes::Move:
			std::cout << "RECEIVED MOVE MESSAGE.\n";
			break;

		case MsgTypes::PingRequest:
			std::cout << "Ping request from peer.\n";
			msg.header.id = MsgTypes::PingAnswer; // change msg id
			Send(msg); // Bounce back message
			break;
		case MsgTypes::PingAnswer:
		{
			std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
			std::chrono::system_clock::time_point timeThen;
			msg >> timeThen;
			std::cout << "Ping answer from peer. Roundtrip time: " << std::chrono::duration<double>(timeNow - timeThen).count() << "\n";
		}
		break;
		default:
			break;
		}
	}

public:
	bool peerDisconnected = false;
};

class PlayerCharacter : public SpriteBase
{
public:
	enum class PlayerID { PlayerOne, PlayerTwo };
	enum class PlayerState { Alive, Dead };
	enum class MoveState { Idle, Left, Right, DashL, DashR };
	enum class AttackState { None, FastPunch, Defend, HitStun, BlockStun };

	PlayerCharacter();
	void InitCharacter(PlayerID id, sf::Vector2f startPos);
	void InitNetworkedCharacter(PlayerID id, sf::Vector2f startPos, std::shared_ptr<CustomPeer> peerRef, bool isLocalCharacter);
	~PlayerCharacter();

	void Update(float dt, sf::Window* wnd); // Update states and apply transformations
	void HandleInput(InputManager* input, float dt); // Register input

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
	Animation anim_defend;
	Animation anim_hurt;

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

	// Network --------------
	// The peer object contains all the network functionality and is shared with the scene class
	std::shared_ptr<CustomPeer> thisPeer;

	bool characterNetworked = false; // Is this character setup as a network match participant?

	enum class NetworkAuthority { Offline, Local, Remote };
	NetworkAuthority networkAuthority; // Assuming multiplayer, is this the local or the remote character?
};

