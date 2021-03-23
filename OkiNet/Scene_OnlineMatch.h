#pragma once
#include <net_common_headers.h> // Network static libraries
#include "GameState.h"
#include "PlayerCharacter.h"
#include <string>

// Holds the IDs to interpret the messages
enum class MsgTypes : uint32_t
{
	Ping,
	Move,
	Dash,
	FastPunch
};

// Create a host client with overridden methods 
class CustomHostClient : public net::HostClient<MsgTypes>
{
public:
	CustomHostClient(uint16_t port) : net::HostClient<MsgTypes>(port)
	{

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

		case MsgTypes::Ping:
			std::cout << "Ping from peer\n";
			MessageClient(msg); // Bounce back message
			break;
		default:
			break;
		}
	}	
};

class CustomClient : public net::Client<MsgTypes>
{
public:
	void Move()
	{
		net::message<MsgTypes> msg;
		msg.header.id = MsgTypes::Move;
		//msg << x;
		Send(msg);
		std::cout << "Sent move message.\n";
	}

	void Ping()
	{
		net::message<MsgTypes> msg;
		msg.header.id = MsgTypes::Ping;
		std::chrono::system_clock::time_point timenow = std::chrono::system_clock::now();
		msg << timenow;
		Send(msg);

	}
};

class Scene_OnlineMatch : public Scene
{
public:
	// Make sure to allow use of the default constructor
	using Scene::Scene;

	void Init(GameState* stateMan); // Initialize gameplay variables needed for any client
	void InitAsHost(GameState* stateMan, std::string& port); // Initialize as host client TODO:: ADD PORT INPUT
	void InitAsClient(GameState* stateMan, std::string& ip, std::string& port); // Initialize attempting to join the address provided
	void OverrideRender();
	void OverrideUpdate(float dt);
	void OverrideHandleInput(float dt);

private:
	GameState* stateManager;

	SpriteBase platform;
	PlayerCharacter playerOne;
	PlayerCharacter playerTwo;

	sf::Vector2f playerOneStartPos;
	sf::Vector2f playerTwoStartPos;

	sf::Font DebugFont;
	sf::Text DebugText;

	// network variables -------------------
	// A host client can act as a normal client as long as it does not start listening for incoming connections
	std::string ip;
	std::string port;

	// TODO -> Adapt the client class to act as host (as they should have the same functionalities)
	CustomHostClient* hostClient = nullptr;
	CustomClient* client = nullptr;

	bool isHost = false; 
	bool remotePlayerConnected = false;

	sf::Vector2f tempPos;

};


