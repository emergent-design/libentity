#pragma once

#include <entity/entity.h>


namespace ent
{
	// A JSON serialiser which converts a tree representing an object to and from
	// a string. All serialisers must contain two static functions:
	// 1. The 'to' function takes a tree and returns a string.
	// 2. The 'from' function takes a string and returns a tree.
	class json
	{
		public:

			// Serialise from a tree to a JSON string
			static std::string to(const tree &item, bool pretty, int depth = 0);

			// Deserialise from a JSON string to a tree
			static tree from(const std::string &text);


		private:

			// String escape, ignores the forward slash (only relevant when dealing with
			// html). Does not handle unicode properly.
			static std::string escape(const std::string item);

			// String unescape, restores escaped characters to their former glory.
			static std::string unescape(const std::string item);

			// Stringify a property value
			static std::string property(const value &item, bool pretty, int depth);

			// Very basic iterative validation. It ensures that all objects and
			// arrays are terminated so that the recursive parsing functions
			// don't fall over.
			static void validate(const std::string &text);

			// In each of the following 'i' is the current position index within
			// the JSON string.

			// Parse a JSON string into an object tree
			static tree parse(const std::string &text, int &i);

			// Extract the key, should be a simple string within quotes
			static std::string parse_key(const std::string &text, int &i);

			// Extract a string value ignoring any escape characters
			static std::string parse_string(const std::string &text, int &i);

			// Extract a numeric/boolean/null value, should always end with a
			// whitespace character, comma or '}'.
			static std::string parse_item(const std::string &text, int &i);

			// Parse an array, contained within square brackets
			static value parse_array(const std::string &text, int &i);

			// Throw an error which shows the whereabouts of a problem in the JSON string.
			static void error(const std::string message, const std::string json, int i);

			// Whitespace lookup table
			static bool whitespace[256];
	};


	// Utility function that lets you cout any entities.
	inline std::ostream &operator << (std::ostream &output, entity &e)
	{
		return output << e.to<json>(true);
	}
}
