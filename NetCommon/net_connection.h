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
		Connection(asio::io_context& _asioContext, asio::ip::tcp::socket _socketTCP, asio::ip::udp::socket _socketUDP, TQueue<message<T>>& _qIn)
			: asioContext(_asioContext), socket_tcp(std::move(_socketTCP)), socket_udp(std::move(_socketUDP)), messagesIn(_qIn)
		{}

		virtual ~Connection()
		{}

		// When the connection joins to a Host peer
		void ConnectTo(const asio::ip::tcp::resolver::results_type& endpoints)
		{
			// ASYNC - Attempt connection
			asio::async_connect(socket_tcp, endpoints,
				[this](std::error_code ec, asio::ip::tcp::endpoint endpoint)
			{
				if (!ec)
				{
					ReadHeader_TCP();
				}
			});
		}

		// "Connect" udp to use async methods without passing endpoints
		void ConnectToUDP()
		{
			// ASYNC - Attempt connection TODO: MAKE ASYNC?
			
			//if (socket_udp.is_open())
			//{
			//	asio::error_code ec;
			//	asio::ip::udp::resolver resolver(asioContext);
			//	socket_udp.connect(endpoints->endpoint(), ec);

			//	if (!ec)
			//	{
			//		ReadHeader_UDP();
			//	}
			//	else std::cout << "UDP Connection failed: " << ec.message() << "\n.";
			//}
		}

		// Asynchronously close the socket so that ASIO can do so when apropriate
		void Disconnect()
		{
			if (IsConnected())
			{
				asio::post(asioContext, [this]() {socket_tcp.close(); socket_udp.close(); });
			}
		}

		bool IsConnected() const
		{
			return socket_tcp.is_open();
		}

		// ASYNC - Send/post a message to the connection
		void Send_TCP(const message<T>& msg)
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
					WriteHeader_TCP();
				}
			});
		}

		// ASYNC - Send/post a message to the connection
		void Send_UDP(const message<T>& msg)
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
					WriteHeader_UDP();
				}
			});
		}

		// If socket is ready, start reading headers
		void Listen_TCP()
		{
			if (socket_tcp.is_open()) ReadHeader_TCP();
		}

	private:
#pragma region TCP
		// ASYNC - Prepare to read the content/body of the message by first checking the header
		void ReadHeader_TCP()
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
						ReadBody_TCP(); // ASYNC
					}
					else // message is empty/ no body
					{
						AddToIncomingMessageQueue_TCP();
					}
				}
				else
				{
					std::cout << "[TCP] Reading message header failed.\n";
					socket_tcp.close();
				}
			});

		}

		// ASYNC - Read body information
		void ReadBody_TCP()
		{
			asio::async_read(socket_tcp, asio::buffer(tempIn.body.data(), tempIn.body.size()),
				[this](std::error_code ec, std::size_t length)
			{
				if (!ec)
				{
					AddToIncomingMessageQueue_TCP();
				}
				else
				{
					std::cout << "[TCP] Reading message body failed.\n";
					socket_tcp.close();
				}
			});
		}

		// ASYNC - Asynchronous write operation of the header
		void WriteHeader_TCP()
		{
			asio::async_write(socket_tcp, asio::buffer(&messagesOut.front().header, sizeof(message_header<T>)),
				[this](std::error_code ec, std::size_t length)
			{
				if (!ec)
				{
					if (messagesOut.front().body.size() > 0) // is there a body to send?
					{
						WriteBody_TCP();
					}
					else // Pop message and check if theres more
					{
						messagesOut.pop_front();

						if (!messagesOut.empty())
						{
							WriteHeader_TCP();
						}
					}
				}
				else
				{
					std::cout << "[TCP] Writing message header failed.\n";
					socket_tcp.close();
				}
			});
		}

		// ASYNC
		void WriteBody_TCP()
		{
			asio::async_write(socket_tcp, asio::buffer(messagesOut.front().body.data(), messagesOut.front().body.size()),
				[this](std::error_code ec, std::size_t length)
			{
				if (!ec)
				{
					messagesOut.pop_front();

					if (!messagesOut.empty())
					{
						WriteHeader_TCP();
					}
				}
				else
				{
					std::cout << "[TCP] Writing message body failed.\n";
					socket_tcp.close();
				}
			});
		}
#pragma endregion

#pragma region UDP
		// ASYNC - Prepare to read the content/body of the message by first checking the header
		void ReadHeader_UDP()
		{
			socket_udp.async_receive(asio::buffer(&tempIn.header, sizeof(message_header<T>)),
				[this](std::error_code ec, std::size_t length)
			{
				if (!ec)
				{
					if (tempIn.header.size > 0) // if the header is larger than 0, there must be a body
					{
						// Allocate enough space to the body based on the header size info
						tempIn.body.resize(tempIn.header.size);
						ReadBody_UDP(); // ASYNC
					}
					else // message is empty/ no body
					{
						AddToIncomingMessageQueue_UDP();
					}
				}
				else
				{
					std::cout << "[UDP] Reading message header failed: " << ec.message() << "\n";
					socket_udp.close();
				}
			});

		}

		// ASYNC - Read body information
		void ReadBody_UDP()
		{
			socket_udp.async_receive(asio::buffer(tempIn.body.data(), tempIn.body.size()),
				[this](std::error_code ec, std::size_t length)
			{
				if (!ec)
				{
					AddToIncomingMessageQueue_UDP();
				}
				else
				{
					std::cout << "[UDP] Reading message body failed.\n";
					socket_udp.close();
				}
			});
		}

		// ASYNC - Asynchronous write operation of the header
		void WriteHeader_UDP()
		{
			socket_udp.async_send(asio::buffer(&messagesOut.front().header, sizeof(message_header<T>)),
				[this](std::error_code ec, std::size_t length)
			{
				if (!ec)
				{
					if (messagesOut.front().body.size() > 0) // is there a body to send?
					{
						WriteBody_UDP();
					}
					else // Pop message and check if theres more
					{
						messagesOut.pop_front();

						if (!messagesOut.empty())
						{
							WriteHeader_UDP();
						}
					}
				}
				else
				{
					std::cout << "[UDP] Writing message header failed: " << ec.message() << ".\n";
					socket_udp.close();
				}
			});
		}

		// ASYNC
		void WriteBody_UDP()
		{
			socket_udp.async_send(asio::buffer(messagesOut.front().body.data(), messagesOut.front().body.size()),
				[this](std::error_code ec, std::size_t length)
			{
				if (!ec)
				{
					messagesOut.pop_front();

					if (!messagesOut.empty())
					{
						WriteHeader_UDP();
					}
				}
				else
				{
					std::cout << "[UDP] Writing message body failed.\n";
					socket_udp.close();
				}
			});
		}
#pragma endregion
		void AddToIncomingMessageQueue_TCP()
		{
			messagesIn.push_back(tempIn);

			ReadHeader_TCP(); // Task ASIO with reading again so that it does not close (has work to do)
		}

		void AddToIncomingMessageQueue_UDP()
		{
			messagesIn.push_back(tempIn);

			ReadHeader_UDP(); // Task ASIO with reading again so that it does not close (has work to do)
		}

	protected:
		// Connection sockets to remote connection
		asio::ip::tcp::socket socket_tcp;
		asio::ip::udp::socket socket_udp;

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