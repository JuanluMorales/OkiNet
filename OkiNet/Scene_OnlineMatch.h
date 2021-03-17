#pragma once
#include <net_common_headers.h> // Network static libraries
#include "GameState.h"
#include "PlayerCharacter.h"
#include <string>

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
		return true;
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

	void Init(GameState* stateMan); // Initialize as host client
	void InitAsClient(GameState* stateMan, std::string& ip, std::string& port); // Initialize attempting to join the address provided
	void OverrideRender();
	void OverrideUpdate(float dt);
	void OverrideHandleInput(float dt);

	bool AttemptConnection();
	void GrabSomeData(asio::ip::tcp::socket& socket);

private:
	GameState* stateManager;

	SpriteBase platform;
	PlayerCharacter playerOne;
	PlayerCharacter playerTwo;

	sf::Vector2f playerOneStartPos;
	sf::Vector2f playerTwoStartPos;

	sf::Font DebugFont;
	sf::Text DebugText;

	// network variables
	std::vector<char> vBuffer; // Large buffer to store messages

	bool isHost = true; // Assume we are the host unless InitAsClient()
	bool PlayerTwoConnected = false;

	std::string ip;
	std::string port;

	//net::HostClient<MsgTypes> client; // A host client can act as a normal client as long as it does not start listening for incoming connections
};

