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
		{
			// Initialize socket udp
			socket_udp = new asio::ip::udp::socket(asioContext);
		}
		virtual ~Connection()
		{
			delete socket_udp;
		}

		// When the connection joins to a Host Client
		void ConnectTo(const asio::ip::tcp::resolver::results_type& endpoints)
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

		// UDP overload
		void ConnectToUDP(const asio::ip::udp::resolver::results_type& endpoints)
		{
			// ASYNC - Attempt connection
			asio::error_code ec;
			socket_udp->connect(endpoints->endpoint(), ec);

			if (!ec) ReadHeaderUDP();

			//socket_udp->async_connect(endpoints->endpoint(),
			//	[this](std::error_code ec, asio::ip::udp::endpoint endpoint)
			//{
			//	if (!ec)
			//	{
			//		ReadHeaderUDP();
			//	}
			//});
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

		// ASYNC - Send/post a message to the connection
		void SendTCP(const message<T>& msg)
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
					WriteHeaderUDP();
				}
			});
		}

		// If socket is ready, start reading headers
		void Listen_TCP()
		{
			if (socket_tcp.is_open()) ReadHeader();
		}

		void Listen_UDP()
		{
			socket_udp = new asio::ip::udp::socket(asioContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), socket_tcp.local_endpoint().port()));
			if (socket_udp->is_open()) ReadHeaderUDP();
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
					std::cout << "[TCP] Reading message header failed.\n";
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
					std::cout << "[TCP] Reading message body failed.\n";
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
					std::cout << "[TCP] Writing message header failed.\n";
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
					std::cout << "[TCP] Writing message body failed.\n";
					socket_tcp.close();
				}
			});
		}

		private:
		// ASYNC - Prepare to read the content/body of the message by first checking the header
		void ReadHeaderUDP()
		{
			socket_udp->async_receive(asio::buffer(&tempIn.header, sizeof(message_header<T>)),
				[this](std::error_code ec, std::size_t length)
			{
				if (!ec)
				{
					if (tempIn.header.size > 0) // if the header is larger than 0, there must be a body
					{
						// Allocate enough space to the body based on the header size info
						tempIn.body.resize(tempIn.header.size);
						ReadBodyUDP(); // ASYNC
					}
					else // message is empty/ no body
					{
						AddToIncomingMessageQueueUDP();
					}
				}
				else
				{
					std::cout << "[UDP] Reading message header failed.\n";
					socket_udp->close();
				}
			});

		}

		// ASYNC - Read body information
		void ReadBodyUDP()
		{
			socket_udp->async_receive(asio::buffer(tempIn.body.data(), tempIn.body.size()),
				[this](std::error_code ec, std::size_t length)
			{
				if (!ec)
				{
					AddToIncomingMessageQueueUDP();
				}
				else
				{
					std::cout << "[UDP] Reading message body failed.\n";
					socket_udp->close();
				}
			});
		}

		// ASYNC - Asynchronous write operation of the header
		void WriteHeaderUDP()
		{
			socket_udp->async_send(asio::buffer(&messagesOut.front().header, sizeof(message_header<T>)),
				[this](std::error_code ec, std::size_t length)
			{
				if (!ec)
				{
					if (messagesOut.front().body.size() > 0) // is there a body to send?
					{
						WriteBodyUDP();
					}
					else // Pop message and check if theres more
					{
						messagesOut.pop_front();

						if (!messagesOut.empty())
						{
							WriteHeaderUDP();
						}
					}
				}
				else
				{
					std::cout << "[UDP] Writing message header failed.\n";
					socket_udp->close();
				}
			});
		}

		// ASYNC
		void WriteBodyUDP()
		{
			socket_udp->async_send(asio::buffer(messagesOut.front().body.data(), messagesOut.front().body.size()),
				[this](std::error_code ec, std::size_t length)
			{
				if (!ec)
				{
					messagesOut.pop_front();

					if (!messagesOut.empty())
					{
						WriteHeaderUDP();
					}
				}
				else
				{
					std::cout << "[UDP] Writing message body failed.\n";
					socket_udp->close();
				}
			});
		}

		void AddToIncomingMessageQueue()
		{
			messagesIn.push_back(tempIn);

			ReadHeader(); // Task ASIO with reading again so that it does not close (has work to do)
		}

		void AddToIncomingMessageQueueUDP()
		{
			messagesIn.push_back(tempIn);

			ReadHeaderUDP(); // Task ASIO with reading again so that it does not close (has work to do)
		}

	protected:
		// Connection sockets to remote connection
		asio::ip::tcp::socket socket_tcp;
		asio::ip::udp::socket* socket_udp;

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