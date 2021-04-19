#pragma once
#include <net_common_headers.h>

// Holds the IDs to interpret the messages
enum class MsgTypes : uint32_t
{
	PingRequest,
	PingAnswer,
	Pressed_A,
	Released_A,
	Pressed_D,
	Released_D,
	Pressed_S,
	Released_S,
	Pressed_Q,
	Released_Q,
	Pressed_E,
	Released_E,
	Pressed_W,
	Released_W
};

// Inherit from the base peer class to override and add functionality
class NetworkPeer : public net::Peer<MsgTypes>
{
public:
	NetworkPeer(uint16_t port) : net::Peer<MsgTypes>(port) 
	{

	}

	void ResetRemotePlayerStatus();

	// Send a ping request to retrieve the roundtrip time
	void PingRequest();

	// Input messages sent to the remote player
	void Pressed_A();
	void Pressed_D();
	void Pressed_S();
	void Pressed_Q();
	void Pressed_E();
	void Pressed_W();

	void Released_A();
	void Released_D();
	void Released_S();
	void Released_Q();
	void Released_E();
	void Released_W();

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
		bool Released_A = false;
		bool Released_D = false;
		bool Released_S = false;
		bool Released_Q = false;
		bool Pressed_E = false;
		bool Released_E = false;
		bool Pressed_W = false;
		bool Released_W = false;
	};

public:
	RemotePlayerStatus remotePlayerStatus;
	bool peerDisconnected = false;
};

