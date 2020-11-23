#pragma once
#include "net_common.h"
// Class that comprises a network message composed of:
// - A message header that contains a template ID for identification and a size 
// - A message body that contains a vector of bytes

// Message header sent at the start of all messages
template <typename T>
struct message_header
{
	T id{};
	uint32_t size = 0; // use unit32 instead of size_t to reduce byte issues
};

template <typename T>
struct message
{
	message_header<T> header{};
	std::vector<uint8_t> body;

	// Return the size of the entire message packet in bytes
	size_t size() const
	{
		return sizeof(message_header<T>) + body.size();
	}

	// Override std::cout for friendlier description of the message
	friend std::ostream& operator << (std::ostream& os, const message<T>& msg)
	{
		os << "ID:" << int(msg.header.id) << " Size:" << msg.header.size;
		return os;
	}

	// Push data into the message buffer (as long as it is trivial to do so)
	template<typename DataType>
	friend message<T>& operator << (message<T>& msg, const DataType& data)
	{
		// check that the type of the data is copyable
		static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to copy");

		// cache size of vector
		size_t i = msg.body.size();
		// resize the vector by the size of the data
		msg.body.resize(msg.body.size() + sizeof(DataType));

		// Copy the data into newly allocated vector space
		std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

		// Return the target message so it can be chained
		return msg;
	}
};
