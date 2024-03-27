#pragma once


namespace ent
{

}


// static void Changes(const string level, const ent::tree &original, const ent::tree &updated, std::vector<ent::tree> &changes)
// 		{
// 			for (auto &k : key_iterator(original.children))
// 			{
// 				if (!updated.contains(k)) continue;

// 				bool differ = false;

// 				switch (original.at(k).get_type())
// 				{
// 					case ent::tree::Type::Object:	Changes(level + k + ":", original.at(k), updated.at(k), changes);	break;
// 					case ent::tree::Type::Array:	differ = original.at(k).as_array() != updated.at(k).as_array();		break;	// For some reason comparing the trees directly does not work in this module, even though it worked in all of the libentity test cases I could think of.
// 					default:						differ = original.at(k) != updated.at(k);							break;
// 				}

// 				if (differ)
// 				{
// 					changes.push_back({
// 						{ "field", level + k },
// 						{ "original", original.at(k) },
// 						{ "new", updated.at(k) }
// 					});
// 				}
// 			}
// 		}
