#include "Scene_OnlineMatch.h"

void Scene_OnlineMatch::Init(GameState* stateMan)
{
	stateManager = stateMan;

	platform.setSize(sf::Vector2f(1500, 100));
	platform.setPosition(-700, 300);
	platform.setFillColor(sf::Color::Green);

	// Setup debug font and text
	DebugFont.loadFromFile("font/arial.ttf");
	DebugText.setFont(DebugFont);
	DebugText.setString("[DEBUG_OUTPUT] SCENE: ONLINE_SCENE");
	DebugText.setCharacterSize(15);
	DebugText.setOrigin(sf::Vector2f(DebugText.getGlobalBounds().width / 2, DebugText.getGlobalBounds().height / 2));
	DebugText.setPosition(sf::Vector2f(0, -200));
	DebugText.setFillColor(sf::Color::Red);


	// Setup player one
	playerOneStartPos = sf::Vector2f(-500, 52);
	playerOne.InitCharacter(PlayerCharacter::PlayerID::PlayerOne, playerOneStartPos);

	// Setup player two in await mode
	PlayerTwoConnected = false;
	playerTwoStartPos = sf::Vector2f(350, 8);
	playerTwo.InitCharacter(PlayerCharacter::PlayerID::PlayerTwo, playerTwoStartPos);

}

bool Scene_OnlineMatch::AttemptConnection()
{
	// ASIO Connection on localhost test
	asio::error_code ec;

	// ASIO interface object
	asio::io_context context;

	// Create a tcp endpoint to connect to (PLACEHOLDER HTTTP EXAMMPLE PAGE)
	asio::ip::tcp::endpoint endpoint(asio::ip::make_address("93.184.216.31", ec), 80);


	// Create socket, the context delivers the implementation
	asio::ip::tcp::socket socket(context);

	// tell socket to connect
	socket.connect(endpoint, ec);
	std::cout << "Attempting connection..." << std::endl;

	// check if there were errors
	if (!ec)
	{
		std::cout << "Connected!" << std::endl;
		DebugText.setString(DebugText.getString() + "\nSuccesful connection from Player 2");
		PlayerTwoConnected = true;
		return true;
	}
	else
	{
		std::cout << "Failed to connect to address:\n" << ec.message() << std::endl;
		return false;
	}

	/*
	if (socket.is_open())
	{
		//// Start reading asynchronously
		//GrabSomeData(socket);

		//// Send a test message/request
		//std::string sRequest =
		//	"GET /index.html HTTP/1.1\r\n"
		//	"Host: example.com\r\n"
		//	"Connection: close\r\n\r\n";

		//socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);

		//// program does soemthing else, while handling data transfer
		//using namespace std::chrono_literals;
		//std::this_thread::sleep_for(20000ms);

		//context.stop();
		//if (thrContext.joinable()) thrContext.join();
	}*/

	return false;
}

void Scene_OnlineMatch::GrabSomeData(asio::ip::tcp::socket& socket)
{
	socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()),
		[&](std::error_code ec, std::size_t length)
		{
			if (!ec) // if theres no error code 
			{
				// Display the amount of bytes
				std::cout << "\n\nRead " << length << " bytes\n\n";

				for (int i = 0; i < length; i++)
					std::cout << vBuffer[i];


				// Call back the method asynchronously, which will wait until theres something to read
				GrabSomeData(socket);
			}
		}
	);
}

void Scene_OnlineMatch::OverrideRender()
{
	// render something to test
	window->draw(platform);

	window->draw(playerOne);

	if (PlayerTwoConnected) 
	{
		window->draw(playerTwo);
	}

	// Render font
	window->draw(DebugText);


}

void Scene_OnlineMatch::OverrideUpdate(float dt)
{
}

void Scene_OnlineMatch::OverrideHandleInput(float dt)
{
	if (input->isKeyDown(sf::Keyboard::Escape)) {
		input->SetKeyUp(sf::Keyboard::Escape);
		window->close();
	}

	// Press C to attempt connection
	if (input->isKeyDown(sf::Keyboard::C))
	{
		AttemptConnection();
	}

	// Placeholder movement
	if (input->isKeyDown(sf::Keyboard::A))
	{
		playerOne.setPosition(playerOne.getPosition().x - playerOne.MoveSpeed * dt, playerOne.getPosition().y);
	}
	if (input->isKeyDown(sf::Keyboard::D))
	{
		playerOne.setPosition(playerOne.getPosition().x + playerOne.MoveSpeed * dt, playerOne.getPosition().y);
	}
}
