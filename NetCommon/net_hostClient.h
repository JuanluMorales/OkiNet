#pragma once
#include "net_common.h"

namespace net
{
	// Base class that implements "Listen" functionalities so that two clients may form a connection
	// Effectively a "server with only 1 connection" component
	template<typename T>
	class HostClient
	{
	public:
		// Initialize the asio acceptor with the endpoint and port (address)
		HostClient(uint16_t port) : asioAcceptor(asioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
		{

		}

		virtual ~HostClient()
		{
			StopListening();
		}

		// Waits for a connection
		bool StartListening()
		{
			try
			{
				WaitForClientConnection(); // issue work to the asio context so it does not close because of lack of work

				// Start the parallel thread with the context running
				threadContext = std::thread([this]() { asioContext.run(); });


			}
			catch (std::exception& e)
			{
				// Output back error
				std::cerr << "[HOST CLIENT] Exception: " << e.what() << "\n";
				return false;
			}

			std::cout << "[HOST CLIENT] Started Listening!\n";
			return true;
		}

		// Attempts to close the asio context
		bool StopListening()
		{
			asioContext.stop();

			// clear the context thread if it was opened
			if (threadContext.joinable()) threadContext.join();

			std::cout << "[Host Client] Stopped\n";
		}

		// ASYNC - Tell asio to wait for connection
		void WaitForClientConnection()
		{
			// Call a lambda function asynchronously
			asioAcceptor.async_accept([this](/*method params*/ std::error_code ec, asio::ip::tcp::socket socket)
			{
				/*Function body*/
				if(!ec)
				{ 
					// Print out address
					std::cout << "[Host Client] Incoming new connection: " << socket.remote_endpoint() << "\n";

					// Create the new connection as a shared ptr
					connection = std::make_shared<Connection<T>>(asioContext, std::move(socket), messagesIn);


					
				}
				else
				{
					// Error while accepting a client
					std::cout << "[Host Client] Incoming new connection error: " << ec.message() << "\n";
				}

				// Make sure asio does not run out of tasks so it does not close
				WaitForClientConnection();
			});
		}

		// Send message to a specific client
		void MessageClient(std::shared_ptr<Connection<T>> client, const message<T>& msg)
		{

		}

		// SERVER FUNCTIONALITY AS CLIENT ONLY HAS 1 CONNECTION AT A TIME
		//// Send message to all clients, allowing to also not send the message to passed in clients
		//void MessageAllClients(const message<T>& msg, std::shared_ptr<Connection<T>> pIgnoredClient = nullptr)
		//{

		//}


	protected:
		// Called when a client connects
		virtual bool OnClientConnect(std::shared_ptr<Connection<T>> client)
		{
			return false;
		}
		// Called when a client disconnects
		virtual bool OnClientDisconnect(std::shared_ptr<Connection<T>> client)
		{
			return false;
		}
		// Called when a message arrives
		virtual void OnMessage(std::shared_ptr<Connection<T>> client)
		{
			
		}

		// Reference to the connection
		std::shared_ptr<Connection<T>> connection;

		// Queue for incoming message packets
		TQueue<message_owner<T>> messagesIn;

		// Declared in order of initialization:
		asio::io_context asioContext;
		std::thread threadContext;
		asio::ip::tcp::acceptor asioAcceptor;
	};
}

