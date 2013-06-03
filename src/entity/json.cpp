#include "entity/json.h"
#include <sstream>


using namespace std;

namespace ent
{
	string escape(string item)
	{
		// TODO: Replace quotes with \" and whitespace with the appropriate \t,\r,\n
		return item;
	}

	string unescape(string item)
	{
		// TODO:
		return item;
	}


	const char quote		= '"';
	bool whitespace[256]	= { false };

	
	string json::to(entity &item, bool pretty, int depth)
	{
		stringstream result;
		string space 	= pretty ? " " : "";
		string line		= pretty ? "\n" : "";
		string indent	= pretty ? string(2 * (depth + 1), ' ') : "";
		int i			= item.properties.size() - 1;

		result << "{" << space << line;

		for (auto &p : item.properties)
		{
			result	<< indent << quote << p.first << quote << ":"
					<< space << property(p.second, pretty, depth)
					<< (i-- ? "," : "") << line;
		}

		result << (pretty ? string(2 * depth, ' ') : "") << "}";

		return result.str();
	}


	string json::property(value &item, bool pretty, int depth)
	{
		stringstream result;
		string line		= pretty ? "\n" : "";
		string indent	= pretty ? string(2 * (depth + 1), ' ') : "";

		if (item.type == vtype::Array)
		{
			int j = item.array.size() - 1;

			result << "[" << line;

			for (auto &i : item.array)
			{
				result	<< indent << string(2, ' ') << property(i, pretty, depth+1) 
						<< (j-- ? "," : "") << line;
			}

			result << indent << "]";
		}
		else switch (item.type)
		{
			case vtype::String:		result << quote << escape(item.string) << quote;	break;
			//case vtype::Float:		result << item.floating;							break;
			//case vtype::Integer:	result << item.integer;								break;
			case vtype::Number:		result << item.number;								break;
			case vtype::Boolean:	result << (item.boolean ? "true" : "false");		break;
			case vtype::Null:		result << "null";									break;
			case vtype::Object:		result << to(*item.object, pretty, depth+1);		break;
			default:				break;	
		}

		return result.str();
	}


	entity json::from(string &text)
	{
		int i=0;

		if (!whitespace[' ']) whitespace[' '] = whitespace['\t'] = whitespace['\r'] = whitespace['\n'] = whitespace[','] = true;
		
		return parse(text, i);
	}


	entity json::parse(string &text, int &i)
	{
		entity result;
		int length = text.length();

		for (; i<length && whitespace[(byte)text[i]]; i++);

		if (i<length && text[i] == '{')
		{
			while (i<length)
			{
				for (i++; i<length && whitespace[(byte)text[i]]; i++);

				if (i < length)
				{
					if (text[i] == '}') break;
					if (text[i] == '"')
					{
						string name = parse_key(text, i);
						for (i++; i<length && text[i] != ':'; i++);
						for (i++; i<length && whitespace[(byte)text[i]]; i++);

						cout << "Name = " << name << endl;

						if (i<length)
						{
							if (text[i] == '{')
							{
								result.set(name, parse(text, i));
							}
							else if (text[i] == '[')
							{

							}
							else if (text[i] == '"')
							{
								result.set(name, unescape(parse_string(text, i)));
							}
							else
							{
								string item = parse_item(text, i);

								if (item == "true") 		result.set(name, true);
								else if (item == "false")	result.set(name, false);
								else if (item == "null")	result.properties[name] = value();
								else 						result.set(name, stod(item));
							}
						}
					}
				}
			}
		}

		return result;
	}

	string json::parse_key(string &text, int &i)
	{
		int start = ++i;

		for (; i<text.length() && text[i] != '"'; i++);

		return text.substr(start, i-start);
	}


	string json::parse_string(string &text, int &i)
	{
		int start	= ++i;
		bool ignore = false;

		for (; i<text.length(); i++)
		{
			if (text[i] == '"' && !ignore) break;
			ignore = text[i] == '\\';
		}

		cout << "String = " << text.substr(start, i-start) << endl;

		return text.substr(start, i-start);
	}


	string json::parse_item(string &text, int &i)
	{
		int start = i;
		for (i++; i<text.length() && !whitespace[(byte)text[i]]; i++);

		cout << "Item = " << text.substr(start, i-start) << endl;

		return text.substr(start, i-start);
	}


	string json::parse_array(string &text, int &i)
	{
		return "";
	}
}

