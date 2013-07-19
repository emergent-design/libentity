#pragma once

#include <entity/entity.h>


namespace ent
{
	// A YAML serialiser which converts a tree representing an object to and from
	// a string. All serialisers must contain two static functions:
	// 1. The 'to' function takes a tree and returns a string.
	// 2. The 'from' function takes a string and returns a tree.
	class yaml
	{
		public:

			// Serialise from a tree to a YAML string
			static std::string to(tree &item, bool pretty, int depth = 0);

			// Deserialise from an YAML string to a tree
			static tree from(const std::string &text);


		private:

			static std::string escape(const std::string item, int depth);

			static std::string simple_escape(const std::string item);

			static std::string unescape(const std::string item);

			// Stringify a property value
			static std::string property(value &item, int depth);
	};
}
