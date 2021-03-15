#pragma once
#include "net_common.h"
#include "net_message.h"
#include "net_threadSafeQueue.h"
#include "net_connection.h"

namespace net
{
	// Responsible for setting up the asio interface/context and its own connection class
	// Responsible for the game's network communication tasks
	template<typename T>
	class Client
	{
		Client() : socket_tcp(context) // Initialise the socket with default client type and the io context assigned to a tcp socket
		{
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
			return false;
		}

		// Disconnect from the connection
		void Disconnect()
		{

		}

		bool IsConnected()
		{
			return false;
		}

		// Return the queue of messages received
		TQueue<message_owner<T>>& GetIncomingMessages()
		{
			return messagesIn;
		}

		// Return the type of client (join or host)
		ClientType GetClientType() { return thisClientType; }

	protected:
		// Context for asio
		asio::io_context context;
		// Thread that enables the context to execute its own commands
		std::thread thrContext;
		// Socket of connection
		asio::ip::tcp::socket socket_tcp;
		// Pointer to the connection once there was a succesful "tcp handshake"
		std::unique_ptr<Connection<T>> connection;

	private:
		// Incoming messages from remote connection
		TQueue<message_owner<T>> messagesIn;
	};
}
