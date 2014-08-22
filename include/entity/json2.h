#pragma once

#include <entity/parser.h>


namespace ent
{
	using std::string;
	using std::vector;

	struct json2 : codec
	{
		// Array items have 0 length name
		virtual inline os &write_name(os &dst, const string &name, int depth) const
		{
			if (!name.empty()) dst << '"' << name << "\":";
			return dst;
		}

		virtual void separator(os &dst, bool last) const											{ if (!last) dst << ","; }
		virtual void object_start(os &dst, const string &name, std::stack<int> &stack) const		{ write_name(dst, name, stack.size()) << '{'; }
		virtual void object_end(os &dst, std::stack<int> &stack) const								{ dst << '}'; }
		virtual void array_start(os &dst, const string &name, std::stack<int> &stack) const			{ write_name(dst, name, stack.size()) << '['; }
		virtual void array_end(os &dst, std::stack<int> &stack) const								{ dst << ']'; }
		virtual void item(os &dst, const string &name, int depth) const								{ write_name(dst, name, depth) << "null"; }
		virtual void item(os &dst, const string &name, bool value, int depth) const					{ write_name(dst, name, depth) << (value ? "true" : "false"); }
		virtual void item(os &dst, const string &name, int32_t value, int depth) const				{ write_name(dst, name, depth) << value; }
		virtual void item(os &dst, const string &name, int64_t value, int depth) const				{ write_name(dst, name, depth) << value; }
		virtual void item(os &dst, const string &name, double value, int depth) const				{ write_name(dst, name, depth) << value; }
		virtual void item(os &dst, const string &name, const vector<byte> &value, int depth) const	{ write_name(dst, name, depth) << '"' << encode64(value) << '"'; }
		virtual void item(os &dst, const string &name, const string &value, int depth) const
		{
			write_name(dst, name, depth) << '"';

			for (auto &c : value)
			{
				switch (c)
				{
					case '"':	dst << "\\\"";	break;
					case '\\':	dst << "\\\\";	break;
					case '\t':	dst << "\\t";	break;
					case '\n':	dst << "\\n";	break;
					case '\r':	dst << "\\r";	break;
					case '\b':	dst << "\\b";	break;
					case '\f':	dst << "\\f";	break;
					default:	dst << c;		break;
				}
			}
			dst << '"';
		}


		// Whitespace characters (space, tab, carriage return, new line, comma)
		const bool whitespace[256] = { 0,0,0,0,0,0,0,0,0,1, 1,0,0,1,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,1,0,0,0,0,0,0,0, 0,0,0,0,1 };


		virtual bool validate(const string &data) const
		{
			return true;
		}


		virtual bool object_start(const string &data, int &i) const
		{
			int length = data.length();

			// Swallow whitespace
			for (; i<length && whitespace[(byte)data[i]]; i++);

			// An object should always start with an opening brace
			return i<length && data[i] == '{';
		}


		virtual bool object_end(const string &data, int &i) const
		{
			return data[i] == '}';
		}


		virtual bool item(const string &data, int &i, string &name, int &type) const
		{
			int length = data.length();

			for (i++; i<length && whitespace[(byte)data[i]]; i++);

			if (i < length)
			{
				// If the end of this object has been found then stop
				// parsing at this level of recursion.
				if (data[i] == '}') return false;

				// An object should always have a quoted string as a
				// property key.
				if (data[i] == '"')
				{
					// Extract the property key
					name = parse_key(data, i);

					// Look for the key/value separator
					for (i++; i<length && data[i] != ':'; i++)
					{
						if (!whitespace[(byte)data[i]]) error("missing key/value separator", data, i);
					}

					// Swallow any whitespace
					for (i++; i<length && whitespace[(byte)data[i]]; i++);

					return true;
				}
				else error("missing object key", data, i);
			}

			return false;
		}


		virtual bool get(const string &data, int &i, int type, bool def) const
		{
			auto c = data[i];
			if (c == '{' || c == '[' || c == '"' || c == '}')
			{
				skip(data, i, type);
				return false;
			}

			auto item = parse_item(data, '}', i);

			return item == "true";
		}


		virtual int32_t get(const string &data, int &i, int type, int32_t def) const
		{
			auto c = data[i];
			if (c == '{' || c == '[' || c == '"' || c == '}')
			{
				skip(data, i, type);
				return 0;
			}

			auto item = parse_item(data, '}', i);

			try 		{ return stoi(item); }
			catch (...)	{ error("value '" + item + "' is not a valid number", data, i); }

			return 0;
		}


		virtual int64_t get(const string &data, int &i, int type, int64_t def) const
		{
			auto c = data[i];
			if (c == '{' || c == '[' || c == '"' || c == '}')
			{
				skip(data, i, type);
				return 0;
			}

			auto item = parse_item(data, '}', i);

			try 		{ return stoll(item); }
			catch (...)	{ error("value '" + item + "' is not a valid number", data, i); }

			return 0;
		}


		virtual double get(const string &data, int &i, int type, double def) const
		{
			auto c = data[i];
			if (c == '{' || c == '[' || c == '"' || c == '}')
			{
				skip(data, i, type);
				return 0.0;
			}

			auto item = parse_item(data, '}', i);

			try 		{ return stod(item); }
			catch (...)	{ error("value '" + item + "' is not a valid number", data, i); }

			return 0.0;
		}


		virtual string get(const string &data, int &i, int type, const string def) const
		{
			if (data[i] == '"')
			{
				auto result = parse_string(data, i);

				// TODO: Unescape here

				return result;
			}

			skip(data, i, type);
			return {};
		}


		virtual vector<byte> get(const string &data, int &i, int type, const vector<byte> def) const
		{
			if (data[i] == '"')
			{
				return decode64(parse_string(data, i));
			}

			skip(data, i, type);
			return {};
		}


		void skip(const string &data, int &i, char open, char close) const
		{
			char c;
			int count	= 1;
			bool quotes	= false;
			bool ignore	= false;

			for (i++; i<data.length() && count; i++)
			{
				c = data[i];

				if (!whitespace[(byte)c])
				{
					if (!quotes && c == open)	count--;
					if (!quotes && c == close)	count++;
					if (!ignore && c == '"')	quotes = !quotes;

					ignore = quotes && c == '\\';
				}
			}
		}


		virtual void skip(const string &data, int &i, int type) const
		{
			std::cout << "skipping" << std::endl;
			auto c = data[i];

			if (c == '{')		skip(data, i, '{', '}');
			else if (c == '[')	skip(data, i, '[', ']');
			else if (c == '"')	parse_string(data, i);
			else				parse_item(data, '}', i);
		}


		string parse_key(const string &data, int &i) const
		{
			int start = ++i;

			for (; i<data.length() && data[i] != '"'; i++);

			return data.substr(start, i-start);
		}


		string parse_string(const string &data, int &i) const
		{
			int start	= ++i;
			bool ignore = false;	// Flag to ensure escaped quotes within the string are ignored

			for (; i<data.length(); i++)
			{
				if (data[i] == '"' && !ignore) break;
				ignore = data[i] == '\\';
			}

			return data.substr(start, i-start);
		}


		string parse_item(const string &data, const char end, int &i) const
		{
			int start = i;
			for (i++; i<data.length() && !whitespace[(byte)data[i]] && data[i] != end; i++);

			// Jump back a character since the parse_array and parse methods expect
			// to swallow whitespace or opening character next so allow it to find
			// an end of array/object.
			return data.substr(start, i-- - start);
		}


		void error(const string &message, const string &json, int i) const
		{
			int tabs	= 0;
			auto prev 	= json.rfind('\n', i);
			auto next 	= json.find('\n', i);
			int start 	= std::max(i-20, (int)prev + 1);
			int length	= next == string::npos ? 50 : next - prev - 1;

			for (int j=start; j<i; j++) tabs += json[j] == '\t';

			throw std::runtime_error(
				"Error parsing json (" + message +
				") here: \n" + json.substr(start, length) +
				"\n" + string(i-start-tabs + tabs*8, '-') + '^'
			);
		}
	};


	struct json2pretty : json2
	{
		// Array items have 0 length name
		virtual inline os &write_name(os &dst, const string &name, int depth) const
		{
			dst << std::string(2 * depth, ' ');
			if (!name.empty()) dst << '"' << name << "\": ";
			return dst;
		}

		virtual void separator(os &dst, bool last) const										{ dst << (last ? "\n" : ",\n"); }
		virtual void object_start(os &dst, const string &name, std::stack<int> &stack) const	{ write_name(dst, name, stack.size()) << "{\n";				stack.push(0); }
		virtual void object_end(os &dst, std::stack<int> &stack) const							{ dst << std::string(2 * (stack.size() - 1), ' ') << '}';	stack.pop(); }
		virtual void array_start(os &dst, const string &name, std::stack<int> &stack) const		{ write_name(dst, name, stack.size()) << "[\n";				stack.push(0); }
		virtual void array_end(os &dst, std::stack<int> &stack) const							{ dst << std::string(2 * (stack.size() - 1), ' ') << ']';	stack.pop(); }
	};
}
