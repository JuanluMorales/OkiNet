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
		Client() : socket_tcp(context)
		{
			// Initialise the socket with default client type and the io context assigned to a tcp socket

		}

		virtual ~Client()
		{
			// Attempt to disconnect if the client is destroyed
			Disconnect();
		}

	public:
		// Connection method that receives a host and a port number 
		bool Connect(const std::string& host, const uint16_t port)
		{
			try
			{
				// Resolve hostname/ip into address
				asio::ip::tcp::resolver resolver(context);
				asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

				// Create connection
				//connection = std::make_unique<Connection<T>>(); // TODO

				// Create the thread to run the context
				thrContext = std::thread([this]() { context.run(); });

			}
			catch (std::exception& e)
			{
				std::cerr << "[Client] Exception: " << e.what() << "\n";
				return false;
			}

			std::cout << "[Client] Succesful connection to host!\n";
			return true;

		}

		// Disconnect from the connection
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
		TQueue<message_owner<T>>& GetIncomingMessages()
		{
			return messagesIn;
		}

	protected:
		// Context for asio
		asio::io_context context;
		// Thread that enables the context to execute its own commands
		std::thread thrContext;
		// Socket of connection
		asio::ip::tcp::socket socket_tcp;
		asio::ip::udp::socket socket_udp;
		// Pointer to the connection once there was a succesful "tcp handshake"
		std::unique_ptr<Connection<T>> connection;

	private:
		// Incoming messages from remote connection
		TQueue<message_owner<T>> messagesIn;
	};
}
