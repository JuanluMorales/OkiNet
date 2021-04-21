#include "NetworkPeer.h"

void NetworkPeer::ResetRemotePlayerStatus()
{
	remotePlayerStatus.Pressed_A = false;
	remotePlayerStatus.Dashed_A = false;
	remotePlayerStatus.Pressed_D = false;
	remotePlayerStatus.Dashed_D = false;
	remotePlayerStatus.Pressed_S = false;
	remotePlayerStatus.Pressed_Q = false;
	remotePlayerStatus.Pressed_E = false;
	remotePlayerStatus.Pressed_W = false;
	remotePlayerStatus.HeavyKicked = false;
	remotePlayerStatus.HeavyPunched = false;
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

void NetworkPeer::Dashed_A()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::Dashed_A;
	Send_UDP(msg);
}

void NetworkPeer::Pressed_D()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::Pressed_D;
	Send_UDP(msg);
}

void NetworkPeer::Dashed_D()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::Dashed_D;
	Send_UDP(msg);
}

void NetworkPeer::HeavyPunched()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::HeavyPunched;
	Send_UDP(msg);
}

void NetworkPeer::HeavyKicked()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::HeavyKicked;
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
	case MsgTypes::Dashed_A:
		remotePlayerStatus.Dashed_A = true;
		break;
	case MsgTypes::Pressed_D:
		remotePlayerStatus.Pressed_D = true;
		break;
	case MsgTypes::Dashed_D:
		remotePlayerStatus.Dashed_D = true;
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
	case MsgTypes::HeavyKicked:
		remotePlayerStatus.HeavyKicked = true;
		break;
	case MsgTypes::HeavyPunched:
		remotePlayerStatus.HeavyPunched = true;
		break;
	default:
		break;
	}
}
