#pragma once
#include "net_common.h"
// Thread safe queue is needed for the queue of messages from and to the clients
// to be only written once at a time because of the asynchronous nature of the ASIO library

namespace net
{
	// Template the queue to allow the use of enum class
	template<typename T>
	class TQueue
	{
	protected:
		std::mutex muxQueue; // To lock the deque and avoid deadlock with
		std::deque<T> deqQueue; // Double ended queue with dynamic size, capable of contracting or expanding 

	public:
		TQueue() = default; // Explicitly defaulted constructor to make sure no parameters can be ever passed in
		TQueue(const TQueue<T>&) = delete; // Do not allow the queue to be copied as it uses mutexes
		virtual ~TQueue() { clear(); } // Call clear() on destructor, must be virtual to call the destructors in the derived classes
		
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
			deqQueue.emplace_back(std::move(item)); // use emplace instead of push back for compiler optimization based on c++ documentation (?)
													// use std::move() for efficient transfer of resources when moving from t to another object
		}

		void push_front(const T& item)
		{
			std::scoped_lock lock(muxQueue);
			deqQueue.emplace_front(std::move(item)); 
		}

		// Remove and return item from front
		T pop_front()
		{
			std::scoped_lock lock(muxQueue);
			// Use move() and store the t item to allow return along with deletion
			auto t = std::move(deqQueue.front()); // Store item in fribt
			deqQueue.pop_front();
			return t;
		}

		T pop_back()
		{
			std::scoped_lock lock(muxQueue);
			auto t = std::move(deqQueue.back());
			deqQueue.pop_back();
			return t;
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
		// Erase all queue
		void clear()
		{
			std::scoped_lock lock(muxQueue);
			deqQueue.clear();
		}

	};
}
