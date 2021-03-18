#pragma once
#include "net_common.h"
#include "net_client.h"

namespace net
{
	// Base class that implements "Listen" functionalities so that two clients may form a connection
	// Effectively a client that also acts as listen "enabler" of a match 
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

		// Waits for a connection, returns true on succesful start
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
		void StopListening()
		{
			asioContext.stop();

			// clear the context thread if it was opened
			if (threadContext.joinable()) threadContext.join();

			std::cout << "[Host Client] Stopped\n";
		}

		// ASYNC - Tell asio to wait for connection
		void WaitForClientConnection()
		{
			// Call lambda function asynchronously
			asioAcceptor.async_accept([this](/*method params*/ std::error_code ec, asio::ip::tcp::socket socket)
			{
				bool succesfulCon = false;
				/*Function body*/
				if(!ec)
				{ 
					// Print out address
					std::cout << "[Host Client] Incoming new connection: " << socket.remote_endpoint() << "\n";

					// HANDLE CONNECTION OBJECT --------------------------
					// Create the new connection as a shared ptr
					std::shared_ptr<Connection<T>> newConn = std::make_shared<Connection<T>>(asioContext, std::move(socket), messagesIn);	

					connection = std::move(newConn);

					connection->ConnectToClient();

					std::cout << "[Host Client] Connection succesful." << "\n";
					succesfulCon = true;

				}
				else
				{
					// Error while accepting a client
					std::cout << "[Host Client] Incoming new connection error: " << ec.message() << "\n";
				}

				// Make sure asio does not run out of tasks so it does not close
				if(!succesfulCon) WaitForClientConnection();
			});
		}

		// Send message to the peer client
		void MessageClient(const message<T>& msg)
		{
			if (connection && connection->IsConnected())
			{
				connection->Send(msg);
			}
			else
			{
				// Client probably disconnected
				OnClientDisconnect(connection);
				connection.reset();
			}
		}

		// Function that will iterate linearly the incoming messages and include them into the message queue
		void Update(size_t maxMsgCount = -1)
		{
			size_t	messageCount = 0;
			while (messageCount < maxMsgCount && !messagesIn.empty())
			{
				auto msg = messagesIn.pop_front();
				
				OnMessageReceived(msg.remote, msg.msg);

				messageCount++;
			}


		}

	protected:
		// Called when a peer connects
		virtual bool OnClientConnect(std::shared_ptr<Connection<T>> client)
		{
			return false;
		}
		// Called when a peer disconnects
		virtual void OnClientDisconnect(std::shared_ptr<Connection<T>> client)
		{

		}
		// Called when a message from the peer arrives
		virtual void OnMessageReceived(std::shared_ptr<Connection<T>> client, message<T>& msg)
		{
			
		}

		// Reference to the connection
		std::shared_ptr<Connection<T>> connection;

		// Queue for incoming message packets
		TQueue<message_owner<T>> messagesIn;

		// Asio requirements. Declared in order of initialization:
		asio::io_context asioContext;
		std::thread threadContext;
		asio::ip::tcp::acceptor asioAcceptor; // To get the peer socket
	};
}

