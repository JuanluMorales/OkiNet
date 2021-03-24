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


class NetworkPeer : public net::Peer<MsgTypes>
{
public:
	NetworkPeer(uint16_t port) : net::Peer<MsgTypes>(port)
	{

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

	// Input messages
	void Pressed_A()
	{
		net::message<MsgTypes> msg;
		msg.header.id = MsgTypes::Pressed_A;
		Send(msg);
	}
	void Pressed_D()
	{
		net::message<MsgTypes> msg;
		msg.header.id = MsgTypes::Pressed_D;
		Send(msg);
	}
	void Pressed_S()
	{
		net::message<MsgTypes> msg;
		msg.header.id = MsgTypes::Pressed_S;
		Send(msg);
	}
	void Pressed_Q()
	{
		net::message<MsgTypes> msg;
		msg.header.id = MsgTypes::Pressed_Q;
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

