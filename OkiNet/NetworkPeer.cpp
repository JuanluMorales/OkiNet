#include "NetworkPeer.h"

void NetworkPeer::ResetRemotePlayerStatus()
{
	remotePlayerStatus.Pressed_A = false;
	remotePlayerStatus.Dashed_A = false;
	remotePlayerStatus.Pressed_D = false;
	remotePlayerStatus.Dashed_D = false;
	//remotePlayerStatus.Pressed_S = false;
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

void NetworkPeer::RestartRequest()
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::RestartRequest;
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

void NetworkPeer::SendPlayerStatus(PlayerStatus& status)
{
	net::message<MsgTypes> msg;
	msg.header.id = MsgTypes::ReceivePlayerState;

	// attach a time stamp as well
	std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();

	msg << status << timeNow;

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

void NetworkPeer::Rollback_Predict()
{
	// grab last frame's remote input and apply it to this frame's input
	remoteDelayedPlayerStatuses.back() = rollbackFrames.back().remoteStatus;
	predictedRemoteStatuses.push_back(remoteDelayedPlayerStatuses.back());

}

int NetworkPeer::Rollback_Restore()
{
	// How many frames we are ahead
	int rollbackAmount = static_cast<int>(predictedRemoteStatuses.size());

	// Search for discrepancies
	bool areEqual = true;
	bool discrepancy = false;
	for (int i = 0; i < rollbackAmount; i++)
	{
		// compare the remote status with the new update
		areEqual = CompareStatuses(rollbackFrames.at(rollbackFrames.size() - 1 - i).remoteStatus, remoteDelayedPlayerStatuses.back());

		if (!areEqual)
		{
			discrepancy = true;
			rollbackAmount = i;
		}
	}

	if (discrepancy)
	{
		int rollbackPos = static_cast<int>(rollbackFrames.size() - 1 - rollbackAmount);

		// restore that frame
		delayedPlayerStatuses.back() = rollbackFrames.at(rollbackPos).localStatus;
		return rollbackAmount;
	}
	else
	{
		// Return 0 frames to resimulate if there was no discrepancy
		return 0;
	}
}

void NetworkPeer::Rollback_Save()
{
	// Cache this frame's state
	FrameStatus currentStatus;

	currentStatus.localStatus = delayedPlayerStatuses.back();
	currentStatus.remoteStatus = remoteDelayedPlayerStatuses.back();

	// Save the frame information
	if (rollbackFrames.size() < ROLLBACK_FRAMES)
	{
		rollbackFrames.push_back(currentStatus);
	}
	else // If we go above the rollback budgetted frames, pop one out so we can save this one
	{
		rollbackFrames.pop_front();
		rollbackFrames.push_back(currentStatus);
	}
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
	{
		std::cout << "Ping request from peer.\n";
		msg.header.id = MsgTypes::PingAnswer; // change msg id
		Send_UDP(msg); // Bounce back message
	}
	break;
	case MsgTypes::PingAnswer:
	{
		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
		std::chrono::system_clock::time_point timeThen;
		msg >> timeThen;
		std::cout << "Ping answer from peer. Roundtrip time: " << std::chrono::duration<double>(timeNow - timeThen).count()
			<< " (" << std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - timeThen).count() << "ms). Ping: "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - timeThen).count() / 2 << " ms." << "\n";
	}
	break;
	case MsgTypes::SyncStateRequest:
	{
		std::cout << "State request from peer.\n";
		// Check their local state for ourselves
		int _remoteHP;
		int _remotePosX;
		msg >> _remotePosX >> _remoteHP;

		// Check the state
		if (remoteHP != _remoteHP) currentSyncState = SyncState::Desync_HP;
		if (remotePosX != _remotePosX)
		{
			if (currentSyncState == SyncState::Desync_HP) currentSyncState = SyncState::Desync_HPandPos;
			else currentSyncState = SyncState::Desync_Pos;
		}

		if (currentSyncState != SyncState::Synced)
		{
			if (currentSyncState == SyncState::Desync_HP) std::cout << "Health Points State desynced!\n";
			if (currentSyncState == SyncState::Desync_Pos)
			{
				int difference = remotePosX - _remotePosX;
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
		int _remoteHP;
		int _remotePosX;
		msg >> _remotePosX >> _remoteHP;

		// Check the state
		if (remoteHP != _remoteHP) currentSyncState = SyncState::Desync_HP;
		if (remotePosX != _remotePosX)
		{
			if (currentSyncState == SyncState::Desync_HP) currentSyncState = SyncState::Desync_HPandPos;
			else currentSyncState = SyncState::Desync_Pos;
		}

		if (currentSyncState != SyncState::Synced)
		{
			if (currentSyncState == SyncState::Desync_HP) std::cout << "Health Points State desynced!\n";
			if (currentSyncState == SyncState::Desync_Pos)
			{
				int difference = remotePosX - _remotePosX;
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
	case MsgTypes::RestartRequest:
		shouldRestartThisFrame = true;
		break;

	case MsgTypes::ReceivePlayerState:
	{
		if (receivedRemoteUpdateThisFrame)
		{
			extraUpdateMessagesReceived += 1;
		}
		else
		{
			extraUpdateMessagesReceived = 0;
		}

		receivedRemoteUpdateThisFrame = true; // Raise the flag of having received notification from the other player (as he should every frame)

		// Interpret the message
		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
		std::chrono::system_clock::time_point timeThen;

		PlayerStatus newRemoteStatus;

		msg >> timeThen >> newRemoteStatus;

		currentNetworkTechnique = newRemoteStatus.techniqueUsing;

		// calculate the time we should delay the remote input locally
		int lagDiff = static_cast<int>(ceil((std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - timeThen).count() / 2) / 16));
		int delayFrames = newRemoteStatus.appliedDelay - lagDiff;
		dynamicDelayFrames = lagDiff; // Update the dynamic delay

		if (currentNetworkTechnique == NetworkTechnique::InputDelay)
		{
			// If the difference of the ping and the input delay is negative, it means we need to move up the delay
			if (delayFrames < 0)
			{
				delayFrames = 0; // As we should have been lockstepping/halting update, we can assume 0 local delay
			}

			newRemoteStatus.appliedDelay = delayFrames;

			remoteDelayedPlayerStatuses.push_back(newRemoteStatus);
		}
		else if (currentNetworkTechnique == NetworkTechnique::Rollback)
		{
			// If the difference of the ping and the input delay is negative, it means we need to move up the delay
			if (delayFrames < 0)
			{
				delayFrames = 0; // As we should have been lockstepping/halting update, we can assume 0 local delay
			}

			//std::cout << "Their local delay: " << newRemoteStatus.appliedDelay << "f. The ping: " << lagDiff << "ms. Our applied delay: " << delayFrames << "f. Amount of remote statuses: " << remoteDelayedPlayerStatuses.size() << ".\n";

			newRemoteStatus.appliedDelay = delayFrames;

			remoteDelayedPlayerStatuses.push_back(newRemoteStatus);
		}
		else
		{
			remotePlayerStatus = newRemoteStatus;
		}
	}
	break;
	default:
		std::cout << "Received a message that was not recognisable.\n";
		break;
	}

}
