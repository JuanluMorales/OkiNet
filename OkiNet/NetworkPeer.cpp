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

void NetworkPeer::ResetLocalPlayerStatus()
{
	localPlayerStatus.Pressed_A = false;
	localPlayerStatus.Dashed_A = false;
	localPlayerStatus.Pressed_D = false;
	localPlayerStatus.Dashed_D = false;
	localPlayerStatus.Pressed_S = false;
	localPlayerStatus.Pressed_Q = false;
	localPlayerStatus.Pressed_E = false;
	localPlayerStatus.Pressed_W = false;
	localPlayerStatus.HeavyKicked = false;
	localPlayerStatus.HeavyPunched = false;
	receivedRemoteUpdateThisFrame = false;
	localInputThisFrame = false;
}

void NetworkPeer::PingRequest()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::PingRequest;
	std::chrono::system_clock::time_point timenow = std::chrono::system_clock::now();
	msg << timenow;
	Send_UDP(msg);

}

void NetworkPeer::SyncStateRequest()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::SyncStateRequest;
	msg << localHP << localPosX;
	Send_UDP(msg);
}


void NetworkPeer::Pressed_A()
{
	localPlayerStatus.Pressed_A = true;
	localInputThisFrame = true;
}

void NetworkPeer::Dashed_A()
{
	localPlayerStatus.Dashed_A = true;
	localInputThisFrame = true;
}

void NetworkPeer::Pressed_D()
{
	localPlayerStatus.Pressed_D = true;
	localInputThisFrame = true;
}

void NetworkPeer::Dashed_D()
{
	localPlayerStatus.Dashed_D = true;
	localInputThisFrame = true;
}

void NetworkPeer::HeavyPunched()
{
	localPlayerStatus.HeavyPunched = true;
	localInputThisFrame = true;
}

void NetworkPeer::HeavyKicked()
{
	localPlayerStatus.HeavyKicked = true;
	localInputThisFrame = true;
}

void NetworkPeer::SendPlayerStatus()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::ReceivePlayerState;
	msg << localPlayerStatus;
	Send_UDP(msg);
}

void NetworkPeer::Pressed_S()
{
	localPlayerStatus.Pressed_S = true;
	localInputThisFrame = true;
}

void NetworkPeer::Pressed_Q()
{
	localPlayerStatus.Pressed_Q = true;
	localInputThisFrame = true;
}

void NetworkPeer::Pressed_E()
{
	localPlayerStatus.Pressed_E = true;
	localInputThisFrame = true;
}

void NetworkPeer::Pressed_W()
{
	localPlayerStatus.Pressed_W = true;
	localInputThisFrame = true;
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
	case MsgTypes::SyncStateRequest:
	{
		std::cout << "State request from peer.\n";
		// Check their local state for ourselves
		float _remoteHP;
		float _remotePosX;
		msg >> _remotePosX >> _remoteHP;

		if (remoteHP > _remoteHP || remoteHP < _remoteHP) currentSyncState = SyncState::Desync_HP;
		if (remotePosX > _remotePosX || remotePosX < _remotePosX)
		{
			if(currentSyncState == SyncState::Desync_HP) currentSyncState = SyncState::Desync_HPandPos;
			else currentSyncState = SyncState::Desync_Pos;			
		}

		if (currentSyncState != SyncState::Synced)
		{
			if (currentSyncState == SyncState::Desync_HP) std::cout << "Health Points State desynced!\n";
			if (currentSyncState == SyncState::Desync_Pos)
			{
				float difference = remotePosX - _remotePosX;
				std::cout << "Position State desynced by " << difference << " units!\n";
			}
			if (currentSyncState == SyncState::Desync_HPandPos) std::cout << "Both HP and Position State desynced!\n";

		}
		else
		{
			std::cout << "State is correctly synced\n";
		}


		// Send our local information to them
		net::message<MsgTypes> msg2;
		msg2.header.id = MsgTypes::SyncStateAnswer;
		msg2 << localHP << localPosX;
		Send_UDP(msg2);
	}
	break;
	case MsgTypes::SyncStateAnswer:
	{
		std::cout << "State answer from peer.\n";
		// Check their local state for ourselves
		float _remoteHP;
		float _remotePosX;
		msg >> _remotePosX >> _remoteHP;

		// Check the state
		if (remoteHP > _remoteHP || remoteHP < _remoteHP) currentSyncState = SyncState::Desync_HP;
		if (remotePosX > _remotePosX || remotePosX < _remotePosX)
		{
			if (currentSyncState == SyncState::Desync_HP) currentSyncState = SyncState::Desync_HPandPos;
			else currentSyncState = SyncState::Desync_Pos;
		}

		if (currentSyncState != SyncState::Synced)
		{
			if (currentSyncState == SyncState::Desync_HP) std::cout << "Health Points State desynced!\n";
			if (currentSyncState == SyncState::Desync_Pos)
			{
				float difference = remotePosX - _remotePosX;
				std::cout << "Position State desynced by " << difference << " units!\n";
			}
			if (currentSyncState == SyncState::Desync_HPandPos) std::cout << "Both HP and Position State desynced!\n";

		}
		else
		{
			std::cout << "State is correctly synced\n";
		}
	}
	break;

	case MsgTypes::ReceivePlayerState:
		receivedRemoteUpdateThisFrame = true; // Raise the flag of having received notification from the other player (as he should every frame)

		// Interpret the message
		PlayerStatus newRemoteStatus;
		msg >> newRemoteStatus;
		remotePlayerStatus = newRemoteStatus;

		if (remotePlayerStatus.Pressed_Q) {
			std::cout << "Remote press of Q\n";
		}

		break;
	}
}
