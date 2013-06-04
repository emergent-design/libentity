#include "entity/json.h"
#include <sstream>
#include <stack>

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
		if (!whitespace[' '])
		{
			whitespace[' '] = whitespace['\t'] = whitespace['\r'] = whitespace['\n'] = whitespace[','] = true;
		}

		validate(text);

		int i=0;
		return parse(text, i);
	}


	// Very basic iterative validation. It ensures that all objects and arrays are
	// terminated so that the recursive parsing functions don't fall over.
	void json::validate(string &text)
	{
		int j;
		char c;
		stack<pair<int,int>> levels;

		int length				= text.length();
		bool quotes				= false;
		bool ignore				= false;
		static string types[]	= { "object", "array" }; //, 	"key",	"string" };
		static char symbols[]	= { '}', ']' }; //,		'"',	'"' };

		for (int i=0; i<length; i++)
		{
			c = text[i];

			if (!whitespace[(byte)c])
			{
				for (j=0; !quotes && j<2; j++)
				{
					if (c == symbols[j])
					{
						if (levels.top().first != j)
						{
							throw runtime_error(
								"Error parsing json (unterminated " + types[levels.top().first] + 
								") around here: \n" + text.substr(max(levels.top().second-10, 0), 50)
							);
						}
						else levels.pop();
					}
				}

				switch (c)
				{
					case '"':	if (!ignore) quotes = !quotes;				break;
					case '{':	if (!quotes) levels.push(make_pair(0, i));	break;
					case '[':	if (!quotes) levels.push(make_pair(1, i));	break;
					//case ':':
				}

				ignore = quotes && c == '\\';
			}
		}

		if (!levels.empty())
		{
			throw runtime_error(
				"Error parsing json (unterminated " + types[levels.top().first] +
				") around here: \n" + text.substr(max(levels.top().second-10, 0), 50)
			);
		}
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

						if (i<length)
						{
							if (text[i] == '{')			result.set(name, parse(text, i));
							else if (text[i] == '[')	result.set(name, parse_array(text, i));
							else if (text[i] == '"')	result.set(name, unescape(parse_string(text, i)));
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
		bool ignore = false;	// Flag to ensure escaped quotes within the string are ignored

		for (; i<text.length(); i++)
		{
			if (text[i] == '"' && !ignore) break;
			ignore = text[i] == '\\';
		}

		return text.substr(start, i-start);
	}


	string json::parse_item(string &text, int &i)
	{
		int start = i;
		for (i++; i<text.length() && !whitespace[(byte)text[i]]; i++);

		return text.substr(start, i-start);
	}


	value json::parse_array(string &text, int &i)
	{
		value result(vtype::Array);
		int length = text.length();

		while (i<length)
		{
			for (i++; i<length && whitespace[(byte)text[i]]; i++);

			if (i < length)
			{
				if (text[i] == ']') 		break;
				if (text[i] == '{')			result.array.emplace_back(make_shared<entity>(parse(text, i)));
				else if (text[i] == '[')	result.array.emplace_back(parse_array(text, i));
				else if (text[i] == '"')	result.array.emplace_back(parse_string(text, i));
				else
				{
					string item = parse_item(text, i);

					if (item == "true") 		result.array.emplace_back(true);
					else if (item == "false")	result.array.emplace_back(false);
					else if (item == "null")	result.array.emplace_back();
					else 						result.array.emplace_back(stod(item));
				}
			}
		}
		
		return result;
	}
}

