#pragma once
#include "net_common.h"
#include "net_message.h"
#include "net_threadSafeQueue.h"
#include "net_connection.h"

namespace net
{
	// Responsible for setting up the asio interface/context and its own connection class
	// Responsible for the game's network communication tasks
	// A Client or Peer encapsulates the functionality needed to connect and disconnect to a host client 
	template<typename T>
	class Client
	{
	public:
		// Initialise the socket with default client type and the io context assigned to a tcp socket
		// Initialize the asio acceptor to listen for the peer connection
		Client(uint16_t port) : socket_tcp(context), asioAcceptor(context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
		{


		}

		virtual ~Client()
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
				WaitForClientConnection(); // issue work to the asio context so it does not close because of lack of work

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
		void WaitForClientConnection()
		{
			// Call lambda function asynchronously
			asioAcceptor.async_accept([this](/*method params*/ std::error_code ec, asio::ip::tcp::socket socket)
			{
				bool succesfulCon = false;
				/*Function body*/
				if (!ec)
				{
					// Print out address
					std::cout << "Incoming connection: " << socket.remote_endpoint() << "\n";

					// HANDLE CONNECTION OBJECT --------------------------
					// Create the new connection as a shared ptr
					std::shared_ptr<Connection<T>> newConn = std::make_shared<Connection<T>>(context, std::move(socket), messagesIn);

					connection = std::move(newConn);

					connection->ConnectToClient();

					std::cout << "Connection succesful." << "\n";
					OnClientConnect();
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
				if (!succesfulCon) WaitForClientConnection();
			});
		}

	public:
		// Connection method that receives a host and a port number 
		bool Connect(const std::string& host, const uint16_t port)
		{
			try
			{
				// Resolve hostname/ip into address
				asio::ip::tcp::resolver resolver(context);
				asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port)); // url magic

				// Create connection
				connection = std::make_unique<Connection<T>>(
					context, asio::ip::tcp::socket(context), messagesIn);

				// Connect to host client
				connection->ConnectToHostClient(endpoints);

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
			}
		}

		// Is the connection active?
		bool IsConnected()
		{
			if (connection)
			{
				return connection->IsConnected();
			}
			else return false;
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
		void Send(const message<T>& msg)
		{
			if (connection && connection->IsConnected())
			{
				connection->Send(msg);
			}
			else
			{
				// Client probably disconnected
				OnClientDisconnect();
				connection.reset();
			}
		}

	protected:
		// Called when a peer connects
		virtual bool OnClientConnect()
		{
			return false;
		}
		// Called when a peer disconnects
		virtual void OnClientDisconnect()
		{

		}
		// Called when a message from the peer arrives
		virtual void OnMessageReceived(message<T>& msg)
		{

		}

	protected:
		// Context for asio
		asio::io_context context;
		// Thread that enables the context to execute its own commands
		std::thread thrContext;

		asio::ip::tcp::acceptor asioAcceptor; // To get the peer socket to connect

		// Socket of connection
		asio::ip::tcp::socket socket_tcp;

		// Pointer to the connection once there was a succesful "tcp handshake"
		std::shared_ptr<Connection<T>> connection;

	private:
		// Incoming messages from remote connection
		TQueue<message<T>> messagesIn;
	};
}
