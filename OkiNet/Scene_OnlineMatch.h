#pragma once
#include <net_common_headers.h> // Network static libraries
#include "GameState.h"
#include "PlayerCharacter.h"
#include <string>

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

	enum class MatchState { Start, OnGoing, P1Win, P2Win, Restart };
	void Restart();
	int restartTime = 200; // Time in frames until restart
	int restartCounter = 0;

	int p1Score = 0;
	int p2Score = 0;

private:
	GameState* stateManager;
	MatchState thisMatchState;

	SpriteBase platform;
	PlayerCharacter localPlayer;
	PlayerCharacter remotePlayer;

	sf::Vector2f playerOneStartPos;
	sf::Vector2f playerTwoStartPos;

	// Map limits
	CollisionBox* leftColl;
	CollisionBox* rightColl;

	// Life bars
	std::vector<sf::RectangleShape*> p1_lifeBar;
	std::vector<sf::RectangleShape*> p1_lifeBarBackground;
	std::vector<sf::RectangleShape*> p2_lifeBar;
	std::vector<sf::RectangleShape*> p2_lifeBarBackground;
	// Energy bars
	std::vector<sf::RectangleShape*> p1_energyBar;
	std::vector<sf::RectangleShape*> p1_energyBarBackground;
	std::vector<sf::RectangleShape*> p2_energyBar;
	std::vector<sf::RectangleShape*> p2_energyBarBackground;

	sf::Text p1ScoreText;
	sf::Text p2ScoreText;

	sf::Font DebugFont;
	sf::Text DebugText;

	// network variables -------------------
	// A host client can act as a normal client as long as it does not start listening for incoming connections
	std::string ip;
	std::string port;

	// The peer object is shared with the character controller for messaging purposes
	std::shared_ptr<NetworkPeer> thisPeer = nullptr;

	bool isHost = false; 
	bool remotePlayerConnected = false;

	sf::Vector2f tempPos;

};


