#include "entity/yaml.h"
#include <sstream>

using namespace std;


namespace ent
{
	string yaml::escape(const string item, int depth)
	{
		stringstream result;
		string indent = string(2 * depth, ' ');

		if ((int)item.find('\n') >= 0)
		{
			result << "|\n";

			int i=0, pos;

			while (true)
			{
				pos = item.find('\n', i);

				if (pos < 0)
				{
					result << indent << item.substr(i);
					break;
				}
				else result << indent << item.substr(i, pos-i+1);

				i = pos+1;
			}
		}
		else result << item;

		return result.str();
	}


	string yaml::simple_escape(const string item)
	{
		string result;
		result.reserve(item.length());

		for (auto &c : item)
		{
			switch (c)
			{
				case '"':	result.append("\\\"");	break;
				case '\\':	result.append("\\\\");	break;
				case '\t':	result.append("\\t");	break;
				case '\n':	result.append("\\n");	break;
				case '\r':	result.append("\\r");	break;
				case '\b':	result.append("\\b");	break;
				case '\f':	result.append("\\f");	break;
				default:	result.append(1, c);	break;
			}
		}

		return result;
	}


	string yaml::unescape(const string item)
	{
		return item;
	}


	string yaml::to(tree &item, bool pretty, int depth)
	{
		stringstream result;
		string indent = string(2 * depth, ' ');

		for (auto &p : item.properties)
		{
			result	<< indent << p.first << ": " << property(p.second, depth);
		}

		return result.str();
	}


	string yaml::property(value &item, int depth)
	{
		stringstream result;

		if (item.type == vtype::Array)
		{
			bool simple		= item.array.size() < 16;
			string indent 	= string(2 * (depth + 1), ' ');

			for (auto &i : item.array)
			{
				if (!simple) break;
				if (i.type == vtype::String && i.string.length() > 32)	simple = false;
				if (i.type == vtype::Object || i.type == vtype::Array)	simple = false;
			}

			if (simple)
			{
				result << "[ ";
				int j = item.array.size() - 1;

				for (auto &i : item.array)
				{
					switch (i.type)
					{
						case vtype::String:		result << '"' << simple_escape(i.string) << '"';	break;
						case vtype::Number:		result << i.number;									break;
						case vtype::Boolean:	result << (i.boolean ? "true" : "false");			break;
						default:				break;
					}
					result << (j-- ? ", " : "");
				}

				result << " ]\n";
			}
			else
			{
				result << "\n";
				
				for (auto &i : item.array)
				{
					result << indent << "- " << property(i, depth+1);
				}
			}
		}
		else switch (item.type)
		{
			case vtype::String:		result << escape(item.string, depth+1) << "\n";			break;
			case vtype::Number:		result << item.number << "\n";							break;
			case vtype::Boolean:	result << (item.boolean ? "true" : "false") << "\n";	break;
			case vtype::Object:		result << "\n" << to(*item.object, false, depth+1);		break;
			default:				result << "\n";											break;
		}

		return result.str();
	}


	tree yaml::from(const string &text)
	{
		return tree();
	}
}
