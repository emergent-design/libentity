#include "entity/json.h"
#include <sstream>
#include <stack>

using namespace std;

namespace ent
{
	
	string json::escape(string item)
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



	string json::unescape(string item)
	{
		bool special = false;
		string result;
		result.reserve(item.length());

		for (auto &c : item)
		{
			if (special) switch (c)
			{
				case '"':	result.append("\"");	break;
				case '\\':	result.append("\\");	break;
				case 't':	result.append("\t");	break;
				case 'n':	result.append("\n");	break;
				case 'r':	result.append("\r");	break;
				case 'b':	result.append("\b");	break;
				case 'f':	result.append("\f");	break;
			}
			else if (c != '\\') result.append(1, c);

			special = c == '\\';			
		}

		return result;
	}



	const char quote			= '"';
	bool json::whitespace[256]	= { false };


	string json::to(tree &item, bool pretty, int depth)
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
		string extra 	= pretty ? string(2, ' ') : "";

		if (item.type == vtype::Array)
		{
			int j = item.array.size() - 1;

			result << "[" << line;

			for (auto &i : item.array)
			{
				result	<< indent << extra << property(i, pretty, depth+1) 
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


	tree json::from(string &text)
	{
		if (!whitespace[' '])
		{
			// Initialise the whitespace lookup if it hasn't been already
			whitespace[' '] = whitespace['\t'] = whitespace['\r'] = whitespace['\n'] = whitespace[','] = true;
		}

		// Check that this looks like valid json
		validate(text);

		int i = 0;
		return parse(text, i);
	}


	void json::validate(string &text)
	{
		int j;
		char c;
		stack<pair<int,int>> levels;

		int length				= text.length();
		bool quotes				= false;
		bool ignore				= false;
		static string types[]	= { "object", "array" };
		static char symbols[]	= { '}', ']' };
		
		for (int i=0; i<length; i++)
		{
			c = text[i];

			if (!whitespace[(byte)c])
			{
				for (j=0; !quotes && j<2; j++)
				{
					if (c == symbols[j])
					{
						// If the terminator symbol does not match the type at the top of
						// the stack then there is an error in the json. Otherwise pop that
						// level off of the stack
						if (levels.top().first != j)
						{
							error("unterminated " + types[levels.top().first], text, levels.top().second);
						}
						else levels.pop();
					}
				}

				// Each time a new object or array is started push the type and
				// position in the string onto a stack.
				switch (c)
				{
					case '"':	if (!ignore) quotes = !quotes;				break;
					case '{':	if (!quotes) levels.push(make_pair(0, i));	break;
					case '[':	if (!quotes) levels.push(make_pair(1, i));	break;
				}

				// If a backslash is found within a string then ignore the next
				// character in case it is a quote.
				ignore = quotes && c == '\\';
			}
		}

		// If there are any levels left in the stack at the end then there was a problem
		// with the json.
		if (!levels.empty()) error("unterminated " + types[levels.top().first], text, levels.top().second);
	}


	tree json::parse(string &text, int &i)
	{
		tree result;
		int length = text.length();

		// Swallow whitespace
		for (; i<length && whitespace[(byte)text[i]]; i++);

		// An object should always start with an opening brace
		if (i<length && text[i] == '{')
		{
			while (i<length)
			{
				// Swallow any whitespace
				for (i++; i<length && whitespace[(byte)text[i]]; i++);

				if (i < length)
				{
					// If the end of this object has been found then stop
					// parsing at this level of recursion.
					if (text[i] == '}') break;

					// An object should always have a quoted string as a
					// property key.
					if (text[i] == '"')
					{
						// Extract the property key
						string name = parse_key(text, i);

						// Look for the key/value separator
						for (i++; i<length && text[i] != ':'; i++)
						{
							if (!whitespace[(byte)text[i]]) error("missing key/value separator", text, i);
						}

						// Swallow any whitespace
						for (i++; i<length && whitespace[(byte)text[i]]; i++);

						if (i<length)
						{
							// Parse the property value based on the next character
							if (text[i] == '{')			result.set(name, parse(text, i));					// Object
							else if (text[i] == '[')	result.properties[name] = parse_array(text, i); 	//result.set(name, parse_array(text, i));				// Array
							else if (text[i] == '"')	result.set(name, unescape(parse_string(text, i)));	// String
							else
							{
								string item = parse_item(text, i);

								if (item == "true") 		result.set(name, true);				// Boolean
								else if (item == "false")	result.set(name, false);			// Boolean
								else if (item == "null")	result.properties[name] = value();	// Null
								else 						result.set(name, stod(item));		// Number
							}
						}
					}
					else error("missing object key", text, i);
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
		for (i++; i<text.length() && !whitespace[(byte)text[i]] && text[i] != '}'; i++);

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
				// If the end of this array has been found then stop
				// parsing at this level of recursion.
				if (text[i] == ']') 		break;
				if (text[i] == '{')			result.array.emplace_back(make_shared<tree>(parse(text, i)));	// Object
				else if (text[i] == '[')	result.array.emplace_back(parse_array(text, i));				// Array
				else if (text[i] == '"')	result.array.emplace_back(unescape(parse_string(text, i)));		// String
				else
				{
					string item = parse_item(text, i);

					if (item == "true") 		result.array.emplace_back(true);		// Boolean
					else if (item == "false")	result.array.emplace_back(false);		// Boolean
					else if (item == "null")	result.array.emplace_back();			// Null
					else 						result.array.emplace_back(stod(item));	// Number
				}
			}
		}
		
		return result;
	}


	void json::error(string message, string json, int i)
	{
		int tabs	= 0;
		auto prev 	= json.rfind('\n', i);
		auto next 	= json.find('\n', i);
		int start 	= max(i-20, (int)prev + 1);
		int length	= next == string::npos ? 50 : next - prev - 1;
		
		for (int j=start; j<i; j++) tabs += json[j] == '\t';

		throw runtime_error(
			"Error parsing json (" + message +
			") here: \n" + json.substr(start, length) +
			"\n" + string(i-start-tabs + tabs*8, '-') + '^'
		);
	}
}

