#pragma once
#include <net_common_headers.h> // Network static libraries
#include "GameState.h"
#include "PlayerCharacter.h"
#include <string>

// Holds the IDs to interpret the messages
enum class MsgTypes : uint32_t
{
	PingRequest,
	PingAnswer,
	Move,
	Dash,
	FastPunch
};

class CustomPeer : public net::Peer<MsgTypes>
{
public:
	CustomPeer(uint16_t port) : net::Peer<MsgTypes>(port)
	{

	}
	void Move()
	{
		net::message<MsgTypes> msg;
		msg.header.id = MsgTypes::Move;
		Send(msg);
		std::cout << "Sent move message.\n";
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

protected:
	virtual bool OnClientConnect()
	{
		return true;
	}

	virtual void OnClientDisconnect()
	{
		std::cout << "Peer disconnected.\n";
	}

	virtual void OnMessageReceived(net::message<MsgTypes>& msg)
	{
		switch (msg.header.id)
		{
		case MsgTypes::Move:
			std::cout << "RECEIVED MOVE MESSAGE.\n";
			break;

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
};

class Scene_OnlineMatch : public Scene
{
public:
	// Make sure to allow use of the default constructor
	using Scene::Scene;

	void Init(GameState* stateMan); // Initialize gameplay variables needed for any client
	void InitAsHost(GameState* stateMan, std::string& port); // Initialize as host peer
	void InitAsClient(GameState* stateMan, std::string& ip, std::string& port); // Initialize attempting to join the address provided
	void OverrideRender();
	void OverrideUpdate(float dt);
	void OverrideHandleInput(float dt);

private:
	GameState* stateManager;

	SpriteBase platform;
	PlayerCharacter localPlayer;
	PlayerCharacter remotePlayer;

	sf::Vector2f playerOneStartPos;
	sf::Vector2f playerTwoStartPos;

	sf::Font DebugFont;
	sf::Text DebugText;

	// network variables -------------------
	// A host client can act as a normal client as long as it does not start listening for incoming connections
	std::string ip;
	std::string port;

	CustomPeer* thisPeer = nullptr;

	bool isHost = false; 
	bool remotePlayerConnected = false;

	sf::Vector2f tempPos;

};


