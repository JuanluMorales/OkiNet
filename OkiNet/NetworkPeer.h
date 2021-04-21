#pragma once
#include <net_common_headers.h>

// Holds the IDs to interpret the messages
enum class MsgTypes : uint32_t
{
	// Request and answer for the ping/roundtrip time
	PingRequest, 
	PingAnswer,
	// For checking if the game is in sync; ask for the local state of the remote player
	HPRequest,
	HPAnswer,
	PosRequest,
	PosAnswer,
	// Input messages sent/received to/from the remote player
	Pressed_A,
	Dashed_A,
	Pressed_D,
	Dashed_D,
	Pressed_S,
	Pressed_Q,
	Pressed_E,
	Pressed_W
};

// Inherit from the base peer class to override and add functionality
class NetworkPeer : public net::Peer<MsgTypes>
{
public:
	NetworkPeer(uint16_t port) : net::Peer<MsgTypes>(port) 
	{

	}

	// Call at the end of the frame to set all remote statuses to false
	// This way the inputs are treated as triggers
	void ResetRemotePlayerStatus();

	// Send a ping request to retrieve the roundtrip time
	void PingRequest();

	// Input messages sent to the remote player
	void Pressed_A();
	void Dashed_A();
	void Pressed_D();
	void Dashed_D();
	void Pressed_S();
	void Pressed_Q();
	void Pressed_E();
	void Pressed_W();

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
		bool Dashed_A = false;
		bool Pressed_D = false;
		bool Dashed_D = false;
		bool Pressed_S = false;
		bool Pressed_Q = false;
		bool Pressed_E = false;
		bool Pressed_W = false;
	};

public:
	RemotePlayerStatus remotePlayerStatus;
	bool peerDisconnected = false;
};

