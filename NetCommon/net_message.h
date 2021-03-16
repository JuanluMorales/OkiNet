#pragma once
#include "net_common.h"
// Class that comprises a network message composed of:
// - A message header that contains a template ID for identification and a size 
// - A message body that contains a vector of bytes
// The use of templating has the purpose of allowing the pass of any enum classes as the message id to define the information to be passed
// Templating the message header has the knock-on effect of requiring all classes that make use of the message to also be templated T

/*
Usage:
- Create an enum class defining the message:
	enum class GameMessage : uint32_t
	{
		FireBullet,
		MovePlayer
	}
- Create a message of that type:
	net::message<GameMessage> gmMsg;
	gmMsg.header.id = GameMessage::FireBullet

	int a = 1;
	bool b = true;

- Push data in:
	gmMsg << a << b;

- Get data out:
	gmMsg >> b >> a;	// Must be done in the opposite write order

*/

namespace net
{
	// Message header sent at the start of all messages
	template <typename T>
	struct message_header
	{
		T id{}; // Templated class to allow any type of enum classes to be used as a message identifier
		uint32_t size = 0; // use unit32 instead of size_t to reduce byte issues
	};

	// Network packet message structure with a header and a body of the message actual content
	// 
	template <typename T>
	struct message
	{
		message_header<T> header{};
		std::vector<uint8_t> body; // Vector byte contents of the message

		// Return the size of the entire message packet in bytes
		size_t size() const
		{
			return sizeof(message_header<T>) + body.size(); // return header and body size
		}

		// Override std::cout for friendlier description of the packet contents
		// This way "std::cout << Message" will output "ID: X  Size: X"
		friend std::ostream& operator << (std::ostream& os, const message<T>& msg)
		{
			os << "ID: " << int(msg.header.id) << " Size: " << msg.header.size << " bytes";
			return os;
		}

		// Serialization
		// Push data into the message buffer (as long as it is possible to do)
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

		// Deserialization
		// Override the >> operator to improve readability when outputting the message contents
		// Use like "Message >> Content"
		template<typename DataType>
		friend message<T>& operator >> (message<T>& msg, DataType& data)
		{
			// check that the type of the data is copyable
			static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to copy");

			// store the location of the first data 
			size_t i = msg.body.size() - sizeof(DataType);

			// copy from vector to variable
			std::memcpy(&data, msg.body.data() + i, sizeof(DataType));

			// shrink vector to remove read bytes
			msg.body.resize(i);

			// recalculate message size
			msg.header.size = msg.size();

			return msg;

		}

	};

	// Forward declare
	template<typename T>
	class Connection;

	// Message that also contains a shared pointer to the connection the message came from
	template <typename T>
	struct message_owner
	{
		std::shared_ptr<Connection<T>> remote = nullptr;
		message<T> msg; // Regular message object
		
		// Overload the output operator again for better visualization of the string
		friend std::ostream& operator<<(std::ostream& os, const message_owner<T>& msg)
		{
			os << msg.msg;
			return os;
		}
	};

}