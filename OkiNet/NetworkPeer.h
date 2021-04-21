#pragma once
#include <net_common_headers.h>

// Holds the IDs to interpret the messages
enum class MsgTypes : uint32_t
{
	// Request and answer for the ping/roundtrip time
	PingRequest, 
	PingAnswer,
	// For checking if the game is in sync; ask for the local state of the remote player
	SyncStateRequest,
	SyncStateAnswer,
	// Input messages sent/received to/from the remote player
	Pressed_A,
	Pressed_D,
	Pressed_S,
	Pressed_Q,
	Pressed_E,
	Pressed_W,
	// Input messages that are a combination of presses and so must be sent individually
	Dashed_A,
	Dashed_D,
	HeavyPunched,
	HeavyKicked
};

// The synchronization state of both peers
enum class SyncState
{
	Synced,
	Desync_HP,
	Desync_Pos,
	Desync_HPandPos
};

// Inherit from the base peer class to override and add functionality
class NetworkPeer : public net::Peer<MsgTypes>
{
public:
	NetworkPeer(uint16_t port) : net::Peer<MsgTypes>(port) 
	{
		currentSyncState = SyncState::Synced;
	}

	// Call at the end of the frame to set all remote statuses to false
	// This way the inputs are treated as triggers
	void ResetRemotePlayerStatus();

	// Send a ping request to retrieve the roundtrip time
	void PingRequest();
	// Send a state request. Pass local player state as argument
	void SyncStateRequest();


	// Input messages sent to the remote player
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

protected:
	// Called when a client connects to this peer
	virtual bool OnPeerConnect();
	// Called when a client disconnects to this peer
	virtual void OnPeerDisconnect();
	// Called when a client receives a message from the remote peer connection
	virtual void OnMessageReceived(net::message<MsgTypes>& msg);

private:
	//Struct that represents the remote player's input status to update the local representation of the remote player
	struct RemotePlayerStatus
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

public:
	RemotePlayerStatus remotePlayerStatus;
	SyncState currentSyncState;
	bool peerDisconnected = false;

	float localHP = 100;
	float localPosX = 0;
	float remoteHP = 100;
	float remotePosX = 0;
};

