#pragma once
#include "net_common.h"
#include "net_threadSafeQueue.h"
#include "net_message.h"

namespace net
{
	// A connection identifies one entity/user relation to another connection (endpoint)
	// Connection will be handled by their respective client so the class needs to be a shared_ptr
	template<typename T>
	class Connection : public std::enable_shared_from_this<Connection<T>> // public inheritance to get a shared ptr from shared_from_this()
	{
	public:
		Connection()
		{}
		virtual ~Connection() 
		{}
		bool ConnectToClient() 
		{}
		bool Disconnect() 
		{}
		bool IsConnected() const 
		{}

		bool Send(const message<T>& msg) {}

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
		TQueue<message_owner<T>>& messagesIn;
	};
}