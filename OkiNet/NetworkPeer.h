#pragma once
#include <net_common_headers.h>
#include <deque>

// Holds the IDs to interpret the messages
enum class MsgTypes : uint32_t
{
	// Request and answer for the ping/roundtrip time
	PingRequest, 
	PingAnswer,
	// For checking if the game is in sync; ask for the local state of the remote player
	SyncStateRequest,
	SyncStateAnswer,
	// Message id to receive the state of the remote player
	ReceivePlayerState

};

// The synchronization state of both peers
enum class SyncState
{
	Synced,
	Desync_HP,
	Desync_Pos,
	Desync_HPandPos
};

enum class NetworkTechnique
{
	None, // Just send the input state and goodluck
	DeterministicLockstep, // Wait for the other player state before updating
	Delay, // Add input delay 
	Rollback // "Rollback" to previous states
};

// Inherit from the base peer class to override and add functionality
class NetworkPeer : public net::Peer<MsgTypes>
{
public:
	NetworkPeer(uint16_t port) : net::Peer<MsgTypes>(port) 
	{
		currentSyncState = SyncState::Synced;
		currentNetworkTechnique = NetworkTechnique::Delay;
	}

	// FRAME CONSTANTS
	const int DELAY_FRAMES = 30; // 3 to 8 is enough, 1 frame = 16 ms of leeway but less responsive input
	const int ROLLBACK_FRAMES = 8; // 5 to 10 is enough

	// Call at the end of the frame to set all remote statuses to false
	// This way the inputs are treated as triggers
	void ResetRemotePlayerStatus();
	void ResetLocalPlayerStatus();

	// Send a ping request to retrieve the roundtrip time
	void PingRequest();
	// Send a state request. Pass local player state as argument
	void SyncStateRequest();


	// Change the player status 
	void Pressed_A();
	void Pressed_D();
	void Pressed_S();
	void Pressed_Q();
	void Pressed_E();
	void Pressed_W();

	void Dashed_A();
	void Dashed_D();
	void HeavyPunched();
	void HeavyKicked();

	void SendPlayerStatus();

protected:
	// Called when a client connects to this peer
	virtual bool OnPeerConnect();
	// Called when a client disconnects to this peer
	virtual void OnPeerDisconnect();
	// Called when a client receives a message from the remote peer connection
	virtual void OnMessageReceived(net::message<MsgTypes>& msg);

public:
	//Struct that represents the remote player's input status to update the local representation of the remote player
	// Sent at the end of the frame 
	struct PlayerStatus
	{
		bool Pressed_A = false;
		bool Pressed_D = false;
		bool Pressed_S = false;
		bool Pressed_Q = false;
		bool Pressed_E = false;
		bool Pressed_W = false;

		bool Dashed_A = false;
		bool Dashed_D = false;

		bool HeavyPunched = false;
		bool HeavyKicked = false;
	};

	PlayerStatus remotePlayerStatus; // Contains the information on the inputs from the remote player for this frame to be applied locally
	PlayerStatus localPlayerStatus; // Contains the information on the inputs from the local player for this frame to be sent to remote
	std::deque<PlayerStatus> delayedPlayerStatuses; // When using input delay, the localPlayerStatus will be stored in these delayed statuses and execute after DELAY_FRAMES have passed
	SyncState currentSyncState;
	NetworkTechnique currentNetworkTechnique;
	bool peerDisconnected = false;
	bool receivedRemoteUpdateThisFrame = false;
	bool localInputThisFrame = false;

	float localHP = 100;
	float localPosX = 0;
	float remoteHP = 100;
	float remotePosX = 0;
};

