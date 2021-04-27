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
	InputDelay, // Add input delay 
	Rollback // "Rollback" to previous states
};

// Inherit from the base peer class to override and add functionality
class NetworkPeer : public net::Peer<MsgTypes>
{
public:

	NetworkPeer(uint16_t port) : net::Peer<MsgTypes>(port) 
	{
		currentSyncState = SyncState::Synced; // Both players are synced when they connect by default
		currentNetworkTechnique = NetworkTechnique::Rollback;
		useDynamicDelay = true;
	}

	// FRAME CONSTANTS
	bool useDynamicDelay; // whether to calculate frame delay every frame or use the fixed delay
	int dynamicDelayFrames = 0; // The dynamic (updated each frame) frames of delay to be used, should vary from 5 to 20 in a succesful manner
	const int DELAY_FRAMES = 5; // Fixed delay input to be used along with rollback. 3 to 8 is enough, 1 frame = 16 ms of leeway but less responsive input
	const int ROLLBACK_FRAMES = 8; // 5 to 10 is enough

	// Call at the end of the frame to set all remote statuses to false
	// This way the inputs are treated as triggers
	void ResetRemotePlayerStatus();
	void ResetLocalPlayerStatus();

	// Send a ping request to retrieve the roundtrip time
	void PingRequest();
	// Send a state request. Pass local player state as argument
	void SyncStateRequest();

	// Set the network technique to deal with latency
	void SetNetworkTechnique(NetworkTechnique nw) { currentNetworkTechnique = nw; }

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

		int appliedDelay = 0; // Amount of delay the remote applied to this frame status on their local machine. Use for remote input delay calculation
	};

	// When using rollback, the frames will be stored as a FrameStatus struct so that the game can recalculate up to the current frame based on the contents of these frames
	struct FrameStatus
	{
		PlayerStatus localStatus;
		PlayerStatus remoteStatus;

		int p1_Health;
		int p1_PosX;

		int p2_Health;
		int p2_PosX;
	};

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

	void SendPlayerStatus(PlayerStatus & status);

	void Rollback_Save(); // In charge of making sure we dont overcap the rollback frames amount set

protected:
	// Called when a client connects to this peer
	virtual bool OnPeerConnect();
	// Called when a client disconnects to this peer
	virtual void OnPeerDisconnect();
	// Called when a client receives a message from the remote peer connection
	virtual void OnMessageReceived(net::message<MsgTypes>& msg);

public:

	NetworkTechnique currentNetworkTechnique; 

	SyncState currentSyncState;

	PlayerStatus remotePlayerStatus; // Contains the information on the inputs from the remote player for this frame to be applied locally
	PlayerStatus localPlayerStatus; // Contains the information on the inputs from the local player for this frame to be sent to remote

	// Input delayed double ended queues
	std::deque<PlayerStatus> delayedPlayerStatuses; // When using input delay, the localPlayerStatus will be stored in these delayed statuses and execute after DELAY_FRAMES have passed
	std::deque<PlayerStatus> remoteDelayedPlayerStatuses; // If we should delay a remote status because it arrived earlier than the remote expected 

	std::deque<FrameStatus> rollbackFrames; // Structure that stores up to ROLLBACK_FRAMES amount of frame information 

	bool peerDisconnected = false;
	bool receivedRemoteUpdateThisFrame = false; // Has the local player received a player status received message this frame?
	bool localInputThisFrame = false; // To save on bandwith when not using lockstep, only send 

	int localHP = 100;
	int localPosX = 0;
	int remoteHP = 100;
	int remotePosX = 0;
};

