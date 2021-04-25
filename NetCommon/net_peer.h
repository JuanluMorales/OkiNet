#pragma once
#include "net_common.h"
#include "net_message.h"
#include "net_threadSafeQueue.h"
#include "net_connection.h"
#include <string>

namespace net
{
	// Responsible for setting up the asio interface/context and its own connection class
	// Responsible for the game's network communication tasks
	// A Client or Peer encapsulates the functionality needed to connect and disconnect to a host client 
	template<typename T>
	class Peer
	{
	public:
		// Initialise the socket with default client type and the io context assigned to a tcp socket
		// Initialize the asio acceptor to listen for the peer connection
		Peer(uint16_t port) : asioAcceptor(context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
		{
			portNumber = port;
		}

		virtual ~Peer()
		{
			// Attempt to disconnect if the client is destroyed
			Disconnect();

			// Close context and thread
			context.stop();
			// clear the context thread if it was opened
			if (thrContext.joinable()) thrContext.join();

		}


	public:
		// Hosting methods ---------------------------------------
		// Waits for a connection, returns true on succesful start and automatically stops accepting once a connection was succesfully stablished
		bool StartListening()
		{
			try
			{
				WaitForConnection(); // issue work to the asio context so it does not close because of lack of work

				// Start the parallel thread with the context running
				thrContext = std::thread([this]() { context.run(); });
			}
			catch (std::exception& e)
			{
				// Output back error
				std::cerr << "Exception: " << e.what() << "\n";
				return false;
			}

			std::cout << "Started as HOST listening for peer!\n";
			return true;
		}

		// ASYNC - Tell asio to wait for connection
		void WaitForConnection()
		{
			// Call lambda function asynchronously
			asioAcceptor.async_accept([this](/*method params*/ std::error_code ec, asio::ip::tcp::socket socket)
			{
				bool succesfulCon = false;
				/*Function body*/
				if (!ec)
				{
					ipAddress = socket.remote_endpoint().address().to_string();
					UDPportNumber = portNumber + 1;
					// Print out address
					std::cout << "Incoming connection: " << socket.remote_endpoint() << "\n";

					// HANDLE CONNECTION OBJECT --------------------------

					asio::ip::udp::socket tempSockUDP(context, asio::ip::udp::endpoint(asio::ip::make_address(ipAddress), UDPportNumber));
					// Create the new connection as a shared ptr
					std::shared_ptr<Connection<T>> newConn = std::make_shared<Connection<T>>(context, std::move(socket), std::move(tempSockUDP), messagesIn);

					connection = std::move(newConn);

					// Start the asynchronous read to work on the background
					connection->Listen_TCP();
					connection->ConnectToUDP(portNumber + 2, ipAddress);

					std::cout << "Connection succesful." << "\n";
					OnPeerConnect();
					succesfulCon = true;

					// Close as theres no need to listen for more connections
					asioAcceptor.close();
				}
				else
				{
					// Error while accepting a client
					std::cout << "Incoming connection error: " << ec.message() << "\n";
				}

				// Make sure asio does not run out of tasks so it does not close
				if (!succesfulCon) WaitForConnection();
			});
		}

	public:
		// Connection method that receives a host and a port number 
		bool Connect(const std::string& host, const uint16_t port)
		{
			try
			{
				ipAddress = host;
				UDPportNumber = portNumber + 2;
				// Resolve hostname/ip into address
				asio::ip::tcp::resolver resolver(context);
				asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port)); // url magic

				asio::ip::udp::socket tempSockUDP(context, asio::ip::udp::endpoint(asio::ip::make_address(host), UDPportNumber));
				// Create connection
				connection = std::make_unique<Connection<T>>(context, asio::ip::tcp::socket(context), std::move(tempSockUDP), messagesIn);

				// Connect to host client
				connection->ConnectTo(endpoints);
				connection->ConnectToUDP(portNumber + 1, ipAddress);
				// Create the thread to run the context
				thrContext = std::thread([this]() { context.run(); });

			}
			catch (std::exception& e)
			{
				std::cerr << "Exception: " << e.what() << "\n";
				return false;
			}

			std::cout << "Succesful connection to peer!\n";
			return true;

		}

		// Disconnect the connection
		void Disconnect()
		{
			if (IsConnected())
			{
				connection->Disconnect();
				OnPeerDisconnect();
			}
		}

		// Is the connection active?
		bool IsConnected()
		{
			if (connection)
			{
				return connection->IsConnected();
			}
			else
			{
				return false;
			}
		}

		// Return the queue of messages received
		TQueue<message<T>>& GetIncomingMessages()
		{
			return messagesIn;
		}

	public:
		// Function that will iterate linearly the incoming messages and include them into the message queue
		void Update()
		{
			if (!messagesIn.empty())
			{
				auto msg = messagesIn.pop_front();

				OnMessageReceived(msg);

			}
		}

		// Send message to connection
		void Send_TCP(const message<T>& msg)
		{
			if (connection && connection->IsConnected())
			{
				connection->Send_TCP(msg);
			}
			else
			{
				// Client probably disconnected
				OnPeerDisconnect();
				connection.reset();
			}
		}

		void Send_UDP(const message<T>& msg)
		{
			if (connection && connection->IsConnected())
			{
				connection->Send_UDP(msg);
			}
			else
			{
				// Peer probably disconnected
				OnPeerDisconnect();
				connection.reset();
			}
		}

	protected:
		// Called when a peer connects
		virtual bool OnPeerConnect()
		{
			return false;
		}
		// Called when a peer disconnects
		virtual void OnPeerDisconnect()
		{

		}
		// Called when a message from the peer arrives
		virtual void OnMessageReceived(message<T>& msg)
		{

		}

		bool IsMessageListEmpty() 
		{ 
			return messagesIn.empty(); 
		}

		void PopFrontMessage()
		{
			messagesIn.pop_front();
		}

		void PopBackMessage()
		{
			messagesIn.pop_back();
		}

	protected:
		// Context for asio
		asio::io_context context;
		// Thread that enables the context to execute its own commands
		std::thread thrContext;

		asio::ip::tcp::acceptor asioAcceptor; // To get the peer socket to connect

		// Pointer to the connection
		std::shared_ptr<Connection<T>> connection;

	private:
		// Incoming messages from remote connection
		TQueue<message<T>> messagesIn;

		uint16_t portNumber;
		uint16_t UDPportNumber; // port number + 10
		std::string ipAddress;

	};
}
