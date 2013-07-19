#pragma once

#include <entity/entity.h>


namespace ent
{
	// An XML serialiser which converts a tree representing an object to and from
	// a string. All serialisers must contain two static functions:
	// 1. The 'to' function takes a tree and returns a string.
	// 2. The 'from' function takes a string and returns a tree.
	class xml
	{
		public:

			// Serialise from a tree to an XML string
			static std::string to(tree &item, bool pretty, int depth = 0);

			// Deserialise from an XML string to a tree
			static tree from(const std::string &text);


		private:

			static std::string escape(const std::string item);

			static std::string unescape(const std::string item);

			// Stringify a property value
			static std::string property(value &item, const std::string name, bool pretty, int depth);

			// In each of the following 'i' is the current position index within
			// the XML string.
			
			// Parse an XML string into an object tree
			static tree parse(const std::string &text, std::string tag, int &i);

			static std::string parse_tag(const std::string &text, int &i);

			// Parse a string (text node)
			static std::string parse_string(const std::string &text, const std::string tag, int &i);

			// Parse an array
			static value parse_array(const std::string &text, const std::string tag, int &i);

			// Extract a numeric/boolean/null value.
			static std::string parse_item(const std::string &text, int &i);

			static void error(const std::string message, const std::string text, int i);
			
			// Whitespace lookup table
			static bool whitespace[256];
	};
}
