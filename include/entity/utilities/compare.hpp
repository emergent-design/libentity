#pragma once

#include <entity/tree.hpp>
#include <entity/entity.hpp>


namespace ent
{
	class compare
	{
		public:

			struct diff
			{
				std::string level;
				ent::tree before;
				ent::tree after;

				emap(eref(level), eref(before), eref(after))
			};


			// Compare two trees and document the changes. It will skip paths that exist in only one of the trees.
			static std::vector<diff> trees(const tree &original, const tree &updated)
			{
				std::vector<diff> changes;

				recurse("", original, updated, changes);

				return changes;
			}


			// Compare two entities and document the changes. It will skip paths that exist in only one of the trees.
			template <typename T> static std::vector<diff> entities(const T &original, const T &updated)
			{
				std::vector<diff> changes;

				recurse("", to_tree(original), to_tree(updated), changes);

				return changes;
			}


		private:

			static void recurse(const string level, const tree &original, const tree &updated, std::vector<diff> &changes)
			{
				for (auto &[name, before] : original.children)
				{
					if (!updated.contains(name))
					{
						continue;
					}

					bool differ = false;
					auto &after = updated.at(name);

					switch (before.get_type())
					{
						case tree::Type::Object:	recurse(level + name + ":", before, after, changes);	break;
						case tree::Type::Array:		differ = before.as_array() != after.as_array();			break;
						default:					differ = before != after;								break;
					}

					if (differ)
					{
						changes.push_back({ level + name, before, after });
					}
				}
			}
	};
}
