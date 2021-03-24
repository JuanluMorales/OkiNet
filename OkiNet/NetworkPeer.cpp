#include "NetworkPeer.h"

void NetworkPeer::PingRequest()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::PingRequest;
	std::chrono::system_clock::time_point timenow = std::chrono::system_clock::now();
	msg << timenow;
	Send(msg);

}

void NetworkPeer::Pressed_A()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::Pressed_A;
	Send(msg);
}

void NetworkPeer::Pressed_D()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::Pressed_D;
	Send(msg);
}

void NetworkPeer::Pressed_S()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::Pressed_S;
	Send(msg);
}

void NetworkPeer::Pressed_Q()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::Pressed_Q;
	Send(msg);
}

bool NetworkPeer::OnPeerConnect()
{
	return true;
}

void NetworkPeer::OnPeerDisconnect()
{
	peerDisconnected = true;
}

void NetworkPeer::OnMessageReceived(net::message<MsgTypes>& msg)
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
