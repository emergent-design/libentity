#include "entity/xml.h"
#include <sstream>

using namespace std;


namespace ent
{
	string xml::escape(const string item)
	{
		string result;
		result.reserve(item.length());

		for (auto &c : item)
		{
			switch (c)
			{
				case '"':	result.append("&quot;");	break;
				case '\'':	result.append("&apos;");	break;
				case '<':	result.append("&lt;");		break;
				case '>':	result.append("&gt;");		break;
				case '&':	result.append("&amp;");		break;
				default:	result.append(1, c);		break;
			}
		}

		return result;
	}


	// String unescape, restores escaped characters to their former glory.
	string xml::unescape(const string item)
	{
		string result;
		int length = item.length();

		result.reserve(length);

		for (int i=0; i<length; i++)
		{
			if (item[i] == '&')
			{
				string name;
				for (i++; i<length && item[i] != ';'; i++) name.append(1, item[i]);

				if (name == "quot") 		result.append("\"");
				else if (name == "apos") 	result.append("'");
				else if (name == "lt")		result.append("<");
				else if (name == "gt")		result.append(">");
				else if (name == "amp")		result.append("&");
				else error("unknown or unterminated escape entity in string", item, i - name.length());
			}
			else result.append(1, item[i]);
		}

		return result;
	}


	bool xml::whitespace[256]	= { false };
	const string header			= u8R"xml(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>)xml";
	
	
	string xml::to(tree &item, bool pretty, int depth)
	{
		stringstream result;
		string line		= pretty ? "\n" : "";
		string indent	= pretty ? string(2 * (depth + 1), ' ') : "";

		if (!depth) result << header << line << "<entity>" << line;

		for (auto &p : item.properties)
		{
			result	<< indent << "<" << p.first << property(p.second, p.first, pretty, depth) << line;
		}

		if (!depth) result << "</entity>";

		return result.str();
	}



	string xml::property(value &item, const string name, bool pretty, int depth)
	{
		stringstream result;
		string line		= pretty ? "\n" : "";
		string indent	= pretty ? string(2 * (depth + 1), ' ') : "";
		string extra 	= pretty ? string(2, ' ') : "";
		string end		= "</" + name + ">";

		if (item.type == vtype::Array)
		{
			result << ">" << line;

			for (auto &i : item.array)
			{
				result << indent << extra << "<_" << property(i, "_", pretty, depth+1) << line;
			}

			result << indent << end;
		}
		else switch (item.type)
		{
			case vtype::String:		result << ">" << escape(item.string) << end;									break;
			case vtype::Number:		result << " value=\"" << item.number << "\" />";								break;
			case vtype::Boolean:	result << " value=\"" << (item.boolean ? "true" : "false") << "\" />";			break;
			case vtype::Null:		result << " value=\"null\" />";													break;
			case vtype::Object:		result << ">" << line << to(*item.object, pretty, depth+1) << indent << end;	break;
			default:				break;
		}

		return result.str();
	}

	
	tree xml::from(const string &text)
	{
		if (!whitespace[' '])
		{
			// Initialise the whitespace lookup if it hasn't been already
			whitespace[' '] = whitespace['\t'] = whitespace['\r'] = whitespace['\n'] = true;
		}


		// Find the actual start of the object data (skipping the xml header)
		int i		= 0;
		int length	= text.length();

		// Swallow whitespace
		for (; i<length && whitespace[(byte)text[i]]; i++);

		// Skip the XML header if there
		if (text.substr(i, 5) == "<?xml")
		{
			for (; i<length && text[i] != '>'; i++);
			i++;
		}

		// Swallow more whitespace
		for (; i<length && whitespace[(byte)text[i]]; i++);

		// Look for the object starting tag
		if (text.substr(i, 8) != "<entity>") error("expecting 'entity' tag", text, i);

		return parse(text, "entity", (i += 8));
	}


	tree xml::parse(const string &text, const string tag, int &i)
	{
		tree result;
		int length = text.length();

		while (i<length)
		{
			// Swallow any whitespace
			for (; i<length && whitespace[(byte)text[i]]; i++);

			if (text[i] == '<')
			{
				if (text[i+1] == '/')
				{
					if (parse_tag(text, ++i) == tag) break;
					error("mismatched end tag", text, i);
				}
				else
				{
					string name = parse_tag(text, i);

					for (; i<length && whitespace[(byte)text[i]]; i++);

					if (text[i] == '>')
					{
						for (i++; i<length && whitespace[(byte)text[i]]; i++);

						if (text.substr(i, 2) == "<_") 	result.properties[name] = parse_array(text, name, i);
						else if (text[i] == '<')		result.set(name, parse(text, name, i));
						else							result.set(name, unescape(parse_string(text, name, i)));
					}
					else if (text.substr(i, 5) == "value")
					{
						string item = parse_item(text, (i+=5));

						if (item == "true") 		result.set(name, true);				// Boolean
						else if (item == "false")	result.set(name, false);			// Boolean
						else if (item == "null")	result.properties[name] = value();	// Null
						else 						result.set(name, stod(item));		// Number
					}
					else error("unknown attribute", text, i);
				}
			}
			else error("expected opening tag", text, i);
		}

		i++;
		return result;
	}


	string xml::parse_tag(const string &text, int &i)
	{
		int start = ++i;

		for (; i<text.length() && text[i] != ' ' && text[i] != '>'; i++);

		return text.substr(start, i-start);
	}



	string xml::parse_string(const string &text, const string tag, int &i)
	{
		int start = i;

		for (; i<text.length() && text[i] != '<'; i++);

		string result = text.substr(start, i-start);

		if (text[i+1] == '/')
		{
			if (parse_tag(text, ++i) != tag) error("mismatched end tag", text, i);
		}
		else error("missing end tag", text, i);

		i++;
		return result;
	}


	value xml::parse_array(const string &text, const string tag, int &i)
	{
		value result(vtype::Array);
		int length = text.length();

		while (i<length)
		{
			// Swallow any whitespace
			for (; i<length && whitespace[(byte)text[i]]; i++);

			if (text[i] == '<')
			{
				if (text[i+1] == '/')
				{
					if (parse_tag(text, ++i) == tag) break;
					error("mismatched end tag", text, i);
				}
				else
				{
					string name = parse_tag(text, i);

					if (name == "_")
					{
						for (; i<length && whitespace[(byte)text[i]]; i++);

						if (text[i] == '>')
						{
							for (i++; i<length && whitespace[(byte)text[i]]; i++);

							if (text.substr(i, 2) == "<_") 	result.array.emplace_back(parse_array(text, "_", i));				// Array
							else if (text[i] == '<')		result.array.emplace_back(make_shared<tree>(parse(text, "_", i)));	// Object
							else							result.array.emplace_back(unescape(parse_string(text, "_", i)));	// String
						}
						else if (text.substr(i, 5) == "value")
						{
							string item = parse_item(text, (i+=5));

							if (item == "true") 		result.array.emplace_back(true);		// Boolean
							else if (item == "false")	result.array.emplace_back(false);		// Boolean
							else if (item == "null")	result.array.emplace_back();			// Null
							else 						result.array.emplace_back(stod(item));	// Number
						}
						else error("unknown attribute", text, i);
					}
					else error("non-item tag inside array", text, i);
				}
			}
		}

		i++;
		return result;
	}


	string xml::parse_item(const string &text, int &i)
	{
		int length = text.length();
		
		for (; i<length && whitespace[(byte)text[i]]; i++);

		if (text[i] == '=')
		{
			for (i++; i<length && whitespace[(byte)text[i]]; i++);

			if (text[i] == '"')
			{
				int start = ++i;

				for (i++; i<text.length() && text[i] != '"'; i++);

				string result = text.substr(start, i-start);

				for (i++; i<text.length() && text[i] == ' ' && text[i] != '/'; i++);

				if (text[i+1] != '>') error("missing end of single tag", text, i);
				i+=2;

				return result;
			}
			else error("missing quotes around property value", text, i);
		}
		else error("missing '=' in property assignment", text, i);

		return "";
	}


	void xml::error(const string message, const string text, int i)
	{
		int tabs	= 0;
		auto prev 	= text.rfind('\n', i);
		auto next 	= text.find('\n', i);
		int start 	= max(i-20, (int)prev + 1);
		int length	= next == string::npos ? 50 : next - prev - 1;

		for (int j=start; j<i; j++) tabs += text[j] == '\t';

		throw runtime_error(
			"Error parsing xml (" + message +
			") here: \n" + text.substr(start, length) +
			"\n" + string(i-start-tabs + tabs*8, '-') + '^'
		);
	}
}
