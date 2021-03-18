#pragma once
#include <net_common_headers.h> // Network static libraries
#include "GameState.h"
#include "PlayerCharacter.h"
#include <string>

// Holds the IDs to interpret the messages
enum class MsgTypes : uint32_t
{
	HostClientAccept
};

// Create a host client with overridden methods 
class CustomHostClient : public net::HostClient<MsgTypes>
{
public:
	CustomHostClient(uint16_t port) : net::HostClient<MsgTypes>(port)
	{

	}

protected:
	virtual bool OnClientConnect(std::shared_ptr<net::Connection<MsgTypes>> client)
	{
		return false;
	}

	virtual void OnClientDisconnect(std::shared_ptr<net::Connection<MsgTypes>> client)
	{
	}

	virtual void OnMessage(std::shared_ptr<net::Connection<MsgTypes>> client, net::message<MsgTypes>& msg)
	{
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

	CustomHostClient* hostClient = nullptr;
	net::Client<MsgTypes>* client = nullptr;

	bool isHost = false; 
	bool PlayerTwoConnected = false;




};


