#pragma once
#include "net_common.h"
#include "net_threadSafeQueue.h"
#include "net_message.h"

namespace net
{
	// A connection identifies one entity/user relation to another connection (endpoint)
	template<typename T>
	class Connection : public std::enable_shared_from_this<Connection<T>> // Inherit publicly to create a shaed pointer to this class
	{
	public:
		Connection(asio::io_context& _asioContext, asio::ip::tcp::socket _socketTCP, TQueue<message<T>>& _qIn)
			: asioContext(_asioContext), socket_tcp(std::move(_socketTCP)), messagesIn(_qIn)
		{}
		virtual ~Connection()
		{}

		// When the connection links to a Client
		void ConnectToClient()
		{
			// Start the asynchronous read to work on the background
			if(socket_tcp.is_open()) ReadHeader();
		}
		// When the connection joins to a Host Client
		void ConnectToHostClient(const asio::ip::tcp::resolver::results_type& endpoints)
		{
			// ASYNC - Attempt connection
			asio::async_connect(socket_tcp, endpoints,
				[this](std::error_code ec, asio::ip::tcp::endpoint endpoint)
			{
				if (!ec)
				{
					ReadHeader();
				}
			});
		}

		// Asynchronously close the socket so that ASIO can do so when apropriate
		void Disconnect()
		{
			if (IsConnected())
			{
				asio::post(asioContext, [this]() {socket_tcp.close(); });
			}
		}

		bool IsConnected() const
		{
			return socket_tcp.is_open();
		}

		// ASYNC - Send/post a message to the connection
		void Send(const message<T>& msg)
		{
			asio::post(asioContext,
				[this, msg]()
			{
				// Check if the queue is empty or not. This is done because of the asynchronous nature of ASIO:
				// Assume if the out queue is not empty, ASIO is working on sending
				bool writingMessage = !messagesOut.empty();
				messagesOut.push_back(msg);
				if (!writingMessage)
				{
					WriteHeader();
				}
			});
		}

	private:
		// ASYNC - Prepare to read the content/body of the message by first checking the header
		void ReadHeader()
		{
			asio::async_read(socket_tcp, asio::buffer(&tempIn.header, sizeof(message_header<T>)),
				[this](std::error_code ec, std::size_t length)
			{
				if (!ec)
				{
					if (tempIn.header.size > 0) // if the header is larger than 0, there must be a body
					{
						// Allocate enough space to the body based on the header size info
						tempIn.body.resize(tempIn.header.size);
						ReadBody(); // ASYNC
					}
					else // message is empty/ no body
					{
						AddToIncomingMessageQueue();
					}
				}
				else
				{
					std::cout << "Reading message header failed.\n";
					socket_tcp.close();
				}
			});

		}

		// ASYNC - Read body information
		void ReadBody()
		{
			asio::async_read(socket_tcp, asio::buffer(tempIn.body.data(), tempIn.body.size()),
				[this](std::error_code ec, std::size_t length)
			{
				if (!ec)
				{
					AddToIncomingMessageQueue();
				}
				else
				{
					std::cout << "Reading message body failed.\n";
					socket_tcp.close();
				}
			});
		}

		// ASYNC - Asynchronous write operation of the header
		void WriteHeader()
		{
			asio::async_write(socket_tcp, asio::buffer(&messagesOut.front().header, sizeof(message_header<T>)),
				[this](std::error_code ec, std::size_t length)
			{
				if (!ec)
				{
					if (messagesOut.front().body.size() > 0) // is there a body to send?
					{
						WriteBody();
					}
					else // Pop message and check if theres more
					{
						messagesOut.pop_front();

						if (!messagesOut.empty())
						{
							WriteHeader();
						}
					}
				}
				else
				{
					std::cout << "Writing message header failed.\n";
					socket_tcp.close();
				}
			});
		}

		// ASYNC
		void WriteBody()
		{
			asio::async_write(socket_tcp, asio::buffer(messagesOut.front().body.data(), messagesOut.front().body.size()),
				[this](std::error_code ec, std::size_t length)
			{
				if (!ec)
				{
					messagesOut.pop_front();

					if (!messagesOut.empty())
					{
						WriteHeader();
					}
				}
				else
				{
					std::cout << "Writing message body failed.\n";
					socket_tcp.close();
				}
			});
		}

		void AddToIncomingMessageQueue()
		{
			messagesIn.push_back(tempIn);

			ReadHeader(); // Task ASIO with reading again so that it does not close (has work to do)
		}

	protected:
		// Connection sockets to remote connection
		asio::ip::tcp::socket socket_tcp;
		//asio::ip::udp::socket socket_udp;

		// Context provided by client or interface
		asio::io_context& asioContext;

		// Messages to be sent to remote side
		TQueue<message<T>> messagesOut;

		// Queue that holds messages received from remote side
		// Stored as a reference, as the remote connection must provide the queue
		TQueue<message<T>>& messagesIn;

		// As incoming messages are asynchronous, store the message until ready
		message<T> tempIn;
	};
}