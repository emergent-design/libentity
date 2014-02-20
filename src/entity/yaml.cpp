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

		if (item.get_type() == value::Type::Array)
		{
			auto array		= item.array();
			string indent 	= string(2 * (depth + 1), ' ');
			bool simple 	= array.size() < 16 && !std::any_of(array.begin(), array.end(), [](value &i) {
				return (i.get_type() == value::Type::String && i.get(string()).length() > 32)
					|| i.get_type() == value::Type::Object
					|| i.get_type() == value::Type::Array
					|| i.get_type() == value::Type::Binary;
			});

			if (simple)
			{
				result << "[ ";
				int j = array.size() - 1;

				for (auto &i : array)
				{
					switch (i.get_type())
					{
						case value::Type::String:	result << '"' << simple_escape(i.get(string())) << '"';	break;
						case value::Type::Number:	result << i.get(0.0);									break;
						case value::Type::Boolean:	result << (i.get(false) ? "true" : "false");			break;
						default:				break;
					}
					result << (j-- ? ", " : "");
				}

				result << " ]\n";
			}
			else
			{
				result << "\n";
				
				for (auto &i : array)
				{
					result << indent << "- " << property(i, depth+1);
				}
			}
		}
		else switch (item.get_type())
		{
			case value::Type::String:	result << escape(item.get(string()), depth+1) << "\n";					break;
			case value::Type::Number:	if (item.get_numtype() == value::Number::Floating) result << item.get(0.0);
											else result << item.get(0.0);
											result << "\n";										
											break;
			case value::Type::Boolean:	result << (item.get(false) ? "true" : "false") << "\n";					break;
			case value::Type::Object:	result << "\n" << to(item.object(), false, depth+1);					break;
			case value::Type::Binary:	result << escape(encode64(item.get(vector<byte>())), depth+1) << "\n";	break;
			default:					result << "\n";															break;
		}

		return result.str();
	}


	tree yaml::from(const string &text)
	{
		return tree();
	}
}
