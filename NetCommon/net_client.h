#pragma once
#include "net_common.h"
#include "net_message.h"
#include "net_threadSafeQueue.h"

namespace net
{
	// Responsible for setting up the asio interface/context and its own connection class
	// Responsible for the game's network communication tasks
	template<typename T>
	class client
	{
		client() : socket_tcp(context) // Initialise the socket with the io context 
		{
			
		}

		virtual ~client()
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

	protected:

		asio::io_context context;
		// Thread that enables the context to execute its own commands
		std::thread thrContext;

		// Socket of connection
		asio::ip::tcp::socket socket_tcp;

		// Pointer to the connection once there was a succesful "tcp handshake"
		std::unique_ptr<connection<T>> connection;

	private:
		// Incoming messages from remote connection
		TQueue<message_owner<T>> messagesIn;
	};
}
