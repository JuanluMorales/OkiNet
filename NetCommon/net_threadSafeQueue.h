#pragma once
#include "net_common.h"
// Thread safe queue is needed for the queue of messages from the connections to be only written once at a time because of the asynchronous nature of the ASIO library

namespace net
{
	// Template the queue to allow the use of enum class
	template<typename T>
	class TQueue
	{
	public:
		TQueue() = default; // Use default constructor
		TQueue(const TQueue<T>&) = delete; // Do not allow the queue to be copied as it uses mutexes
		
	public:
		// Add guarding to the standard functions the queue would have so they return safely
		const T& front()
		{
			std::scoped_lock lock(muxQueue);
			return deqQueue.front();
		}
		const T& back()
		{
			std::scoped_lock lock(muxQueue);
			return deqQueue.back();
		}

		void push_back(const T& item)
		{
			std::scoped_lock lock(muxQueue);
			return deqQueue.emplace_back(std::move(item)); // use emplace instead of push back for compiler optimization based on c++ documentation (?)
		}

		void push_front(const T& item)
		{
			std::scoped_lock lock(muxQueue);
			return deqQueue.emplace_front(std::move(item)); 
		}

		// Other helpful functions ---------------
		// Return if the queue is empty or not
		bool empty()
		{
			std::scoped_lock lock(muxQueue);
			return deqQueue.empty();
		}
		// Return queue size
		size_t count()
		{
			std::scoped_lock lock(muxQueue);
			return deqQueue.size();
		}
		// Flush clear the queue
		void clear()
		{
			std::scoped_lock lock(muxQueue);
			deqQueue.clear();
		}

	protected:
		std::mutex muxQueue; // To lock the deque and avoid deadlock with
		std::deque<T> deqQueue; // Double ended queue with dynamic size, capable of contracting or expanding 
	};
}
