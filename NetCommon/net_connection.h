#pragma once
#include "net_common.h"
#include "net_message.h"
#include "net_threadSafeQueue.h"

// A connection identifies one entity/user relation to another connection (endpoint)
namespace net
{
	template<typename T>
	class connection : public std::enable_shared_from_this<connection<T>> // public inheritance to get a shared ptr from this
	{
	public:
		connection()
		{}

		virtual ~connection()
		{}

		bool ConnectToClient();
		bool ConnectToServer(); // In case a future server were to be implemented
		bool Disconnect();
		bool IsConnected() const;

		bool Send(const message<T>& msg);

	protected:
		// Connection socket to remote connection
		asio::ip::tcp::socket socket_tcp;
		asio::ip::udp::socket socket_udp;

		// Context provided by client or interface
		asio::io_context& asioContext;

		// Messages to be sent to remote side
		TQueue<message<T>> messagesOut;

		// Queue that holds messages received from remote side
		// Stored as a reference, as the remote connection must provide the queue
		TQueue<message_owner>& messagesIn;
	};
}