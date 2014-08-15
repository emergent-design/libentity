#include "entity/parser.h"
// #include <sstream>

using namespace std;


namespace ent
{
	// string parser::to(const tree &item, bool pretty, int depth)
	// {
	// 	return {};
	// }

/*/*
string result;
		string space 	= pretty ? " " : "";
		string line		= pretty ? "\n" : "";
		string indent	= pretty ? string(2 * (depth + 1), ' ') : "";
		int i			= item.properties.size() - 1;

for (auto &p : item.properties)
		{
			result.append(indent).append(quote).append(p.first).append(quote).append(":").append(space);
			property(result, p.second, pretty, depth);
			if (i--) result.append(",");
			result.append(line);
		}
*/

/*	string parser::to(entity2 &item, bool pretty)
	{

		auto &codec = this->get_codec();

		//string result;
		os result;

		codec.initialise(result, pretty);

		//this->object(codec, result, item, pretty, depth);

		vref<entity2>::encode(item, codec, result, pretty, 0);

		codec.finalise(result, pretty);

		//this->initialise(result);
		// codec.initialise(result, pretty, depth);

		// for (auto &v : map.lookup)
		// {
		// 	//cout << (int)v.second->type << endl;

		// 	codec.key(result, v.first, pretty, depth + 1);

		// 	switch (v.second->type)
		// 	{
		// 		case vtype::Simple: v.second->encode(codec, result);	break;
		// 		case vtype::Object: result <<
		// 	}

		// 	codec.separator(result, !i--, pretty, depth);
		// }

		// codec.finalise(result, pretty, depth);

		return result.str();
	}*/


	/*void parser::object(codec &codec, os &result, entity2 &item, bool pretty, int depth)
	{
		auto map	= item.map();
		int i		= map.lookup.size() - 1;

		codec.object_start(result, pretty, depth);

		for (auto &v : map.lookup)
		{
			codec.key(result, v.first, pretty, depth + 1);
			v.second->encode(codec, result, pretty, depth + 1);

			// switch (v.second->type)
			// {
			// 	case vtype::Simple: v.second->encode(codec, result, pretty, depth + 1);	break;
			// 	case vtype::Object: this->object(codec, result, *static_cast<entity2 *>(v.second->raw()), pretty, depth + 1);	break;
			// }*

			codec.separator(result, !i--, pretty, depth);
		}

		codec.object_end(result, pretty, depth);
	}*/




	// tree from(const std::string &text)
	// {
	// 	return {};
	// }
}
