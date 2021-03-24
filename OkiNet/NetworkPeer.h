#pragma once
#include <net_common_headers.h>

// Holds the IDs to interpret the messages
enum class MsgTypes : uint32_t
{
	PingRequest,
	PingAnswer,
	Pressed_A,
	Pressed_D,
	Pressed_S,
	Pressed_Q,
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
	};

public:
	RemotePlayerStatus remotePlayerStatus;
	bool peerDisconnected = false;
};

