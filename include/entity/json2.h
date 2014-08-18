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
