#include <net_common_headers.h>

enum class CustomMsgTypes : uint32_t
{
	Move,
	Punch,
	Defend
};

//class TestClient : public net::Client<CustomMsgTypes>
//{
//public:
//	bool Move(float x)
//	{
//		net::message<CustomMsgTypes> msg;
//		msg.header.id = CustomMsgTypes::Move;
//		msg << x;
//		Send(msg);
//	}
//};
//
//int main()
//{
//	TestClient c;
//	c.Connect(ip, port);
//	c.Move(player.getPosition().x);
//	return;
//}

std::vector<char> vBuffer(20 * 1024); // Large buffer to store messages

void GrabSomeData(asio::ip::tcp::socket& socket)
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

int asioTest()
{
	net::message<CustomMsgTypes> msg;
	msg.header.id = CustomMsgTypes::Move;
	float x = 5;

	asio::error_code ec; 

	// ASIO interface object
	asio::io_context context;
	// Create fake tasks to asio so the context doesnt finish to allow asynchronicity
	asio::io_context::work idleWork(context);
	// Start the context in a new thread so there is no blocking
	std::thread thrContext = std::thread([&]() {context.run(); });
	
	// Create a tcp endpoint to connect to
	asio::ip::tcp::endpoint endpoint(asio::ip::make_address("93.184.216.34", ec), 80);

	// Create socket, the context delivers the implementation
	asio::ip::tcp::socket socket(context);

	// tell socket to connect
	socket.connect(endpoint, ec);

	// check if there were errors
	if (!ec)
	{
		std::cout << "Connected!" << std::endl;
	}
	else
	{
		std::cout << "Failed to connect to address:\n" << ec.message() << std::endl;
	}

	if (socket.is_open())
	{
		// Start reading asynchronously
		GrabSomeData(socket);

		// Send a test message/request
		std::string sRequest =
			"GET /index.html HTTP/1.1\r\n"
			"Host: example.com\r\n"
			"Connection: close\r\n\r\n";

		socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);

		// program does soemthing else, while handling data transfer
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(20000ms);

		context.stop();
		if (thrContext.joinable()) thrContext.join();
	}


	system("PAUSE");

	return 0;
}
