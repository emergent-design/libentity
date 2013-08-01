#pragma once

#include <entity/entity.h>
#include <sstream>


namespace ent
{
	typedef std::stringstream ss;
	
	// A BSON serialiser which converts a tree representing an object to and from
	// a binary string.
	class bson
	{
		public:

			// Serialise from a tree to a BSON binary string
			static std::string to(const tree &item, bool pretty, int depth = 0);

			// Deserialise from a BSON binary string to a tree
			static tree from(const std::string &text);


		private:

			// Stream writing functions
			static void property(ss &stream, const std::string &name, const value &item);	// Property
			static void key(ss &stream, byte type, const std::string &name);				// Key string
			static void write(ss &stream, int data);										// 32-bit int
			static void write(ss &stream, long long data);									// 64-bit int
			static void write(ss &stream, double data);										// Floating point
			static void write(ss &stream, const std::string &data);							// String
			static void write(ss &stream, const std::vector<byte> &data);					// Binary data
			static void object(ss &stream, tree &item);										// Object tree
			static void number(ss &stream, const std::string &name, const value &item);		// Number
			static void array(ss &stream, const std::vector<value> &items);					// Array


			// Structure that represents a position in the binary data. It includes helper
			// functions to read various value types from the data.
			struct blob
			{
				byte *data;		// Pointer to the start of the data
				byte *current;	// Pointer to the current position
				int remaining;	// Remaining length

				blob(byte *data, int length) : data(data), current(data), remaining(length) {}

				byte next();					// Get the next byte in the blob
				int int32();					// Interpret the next 4 bytes as a 32-bit LE int
				long long int64();				// Interpret the next 8 bytes as a 64-bit LE int
				double floating();				// Interpret the next 8 bytes as an 64-bit LE double
				std::string cstring();			// Read a null-terminated string
				std::string string();			// Read a length based string
				std::vector<byte> binary();		// Read a block of binary data

				// Increments the current pointer, updates the remaining length
				// and then returns the position prior to modification.
				byte *increment(int amount);
			};

			// Parse the data into an object tree
			static tree parse(blob &b);

			// Parse a single object property
			static value parse_property(blob &b, byte type);

			// Parse an array document
			static value parse_array(blob &b);

			// Throw a parsing error and the location within the binary data
			static int error(const std::string message, blob &b);
	};
}
