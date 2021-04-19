#include "NetworkPeer.h"

void NetworkPeer::ResetRemotePlayerStatus()
{
	remotePlayerStatus.Pressed_A = false;
	remotePlayerStatus.Pressed_D = false;
	remotePlayerStatus.Pressed_S = false;
	remotePlayerStatus.Pressed_Q = false;
	remotePlayerStatus.Pressed_E = false;
	remotePlayerStatus.Pressed_W = false;
	remotePlayerStatus.Released_A = false;
	remotePlayerStatus.Released_D = false;
	remotePlayerStatus.Released_Q = false;
	remotePlayerStatus.Released_S = false;
	remotePlayerStatus.Released_E = false;
	remotePlayerStatus.Released_W = false;
}

void NetworkPeer::PingRequest()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::PingRequest;
	std::chrono::system_clock::time_point timenow = std::chrono::system_clock::now();
	msg << timenow;
	Send_UDP(msg);

}

void NetworkPeer::Pressed_A()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::Pressed_A;
	Send_UDP(msg);
}

void NetworkPeer::Pressed_D()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::Pressed_D;
	Send_UDP(msg);
}

void NetworkPeer::Pressed_S()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::Pressed_S;
	Send_UDP(msg);
}

void NetworkPeer::Pressed_Q()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::Pressed_Q;
	Send_UDP(msg);
}

void NetworkPeer::Pressed_E()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::Pressed_E;
	Send_UDP(msg);
}

void NetworkPeer::Pressed_W()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::Pressed_W;
	Send_UDP(msg);
}


void NetworkPeer::Released_A()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::Released_A;
	Send_UDP(msg);
}
void NetworkPeer::Released_D()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::Released_D;
	Send_UDP(msg);
}
void NetworkPeer::Released_S()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::Released_S;
	Send_UDP(msg);
}
void NetworkPeer::Released_Q()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::Released_Q;
	Send_UDP(msg);
}

void NetworkPeer::Released_E()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::Released_E;
	Send_UDP(msg);
}

void NetworkPeer::Released_W()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::Released_W;
	Send_UDP(msg);
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
		Send_UDP(msg); // Bounce back message
		break;
	case MsgTypes::PingAnswer:
	{
		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
		std::chrono::system_clock::time_point timeThen;
		msg >> timeThen;
		std::cout << "Ping answer from peer. Roundtrip time: " << std::chrono::duration<double>(timeNow - timeThen).count() << "\n";
	}
	break;

	case MsgTypes::Pressed_A:
		remotePlayerStatus.Pressed_A = true;
		break;
	case MsgTypes::Pressed_D:
		remotePlayerStatus.Pressed_D = true;
		break;
	case MsgTypes::Pressed_S:
		remotePlayerStatus.Pressed_S = true;
		break;
	case MsgTypes::Pressed_Q:
		remotePlayerStatus.Pressed_Q = true;
		break;
	case MsgTypes::Pressed_E:
		remotePlayerStatus.Pressed_E = true;
		break;
	case MsgTypes::Pressed_W:
		remotePlayerStatus.Pressed_W = true;
		break;
	case MsgTypes::Released_A:
		remotePlayerStatus.Released_A = true;
		break;
	case MsgTypes::Released_D:
		remotePlayerStatus.Released_D = true;
		break;
	case MsgTypes::Released_S:
		remotePlayerStatus.Released_S = true;
		break;
	case MsgTypes::Released_Q:
		remotePlayerStatus.Released_Q = true;
		break;
	case MsgTypes::Released_E:
		remotePlayerStatus.Released_E = true;
		break;
	case MsgTypes::Released_W:
		remotePlayerStatus.Released_W = true;
		break;
	default:
		break;
	}
}
