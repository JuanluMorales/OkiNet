#pragma once
#include "net_common.h"

namespace net
{
	// "Listen" functionalities so that two clients may form a connection
	// Effectively a "small server" component
	template<typename T>
	class HostClient
	{
	public:
		HostClient(uint16_t port)
		{

		}

		virtual ~HostClient()
		{

		}

		bool StartListening()
		{

		}

		bool StopListening()
		{

		}

		// ASYNC - Tell asio to wait for connection
		void WaitForClientConnection()
		{

		}

		void MessageClient(std::shared_ptr<Connection<T>> client, const message<T>& msg)
		{

		}
	};
}

