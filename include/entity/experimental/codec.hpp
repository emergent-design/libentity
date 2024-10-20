#pragma once

#if __cplusplus >= 202002L

#include <concepts>
#include <sstream>
#include <stack>
#include <span>

namespace ent
{

	template <typename T> concept Codec = requires(std::ostringstream &dst, std::string_view name, std::stack<int> &stack, int depth)
	{
		// Encoding
		{ T::separator(dst, false) }			-> std::same_as<void>;
		{ T::object_start(dst, name, stack) }	-> std::same_as<void>;
		{ T::object_end(dst, stack) }			-> std::same_as<void>;
		{ T::array_start(dst, name, stack) }	-> std::same_as<void>;
		{ T::array_end(dst, stack) }			-> std::same_as<void>;

		{ T::item(dst, name, depth ) }							-> std::same_as<void>;
		{ T::item(dst, name, false, depth) }					-> std::same_as<void>;
		{ T::item(dst, name, int32_t {}, depth) }				-> std::same_as<void>;
		{ T::item(dst, name, uint32_t {}, depth) }				-> std::same_as<void>;	// Implementations should cast to int64_t for storage to avoid ambiguity and retain positive values
		{ T::item(dst, name, int64_t {}, depth) }				-> std::same_as<void>;
		{ T::item(dst, name, double {}, depth) }				-> std::same_as<void>;
		{ T::item(dst, name, std::string_view {}, depth) }		-> std::same_as<void>;
		{ T::item(dst, name, std::span<uint8_t> {}, depth) }	-> std::same_as<void>;

	}
	&& requires(std::string_view data, const int type)
	{
		// Decoding
		{ T::validate(data) }			-> std::same_as<bool>;
		{ T::object_start(data, type) }	-> std::same_as<bool>;
	};


	struct json
	{


		// Array items have 0 length name
		static auto &write_name(std::ostringstream &dst, std::string_view name, int)
		{
			if (!name.empty())
			{
				dst << '"' << name << "\":";
			}

			return dst;
		}

		static void write_number(std::ostringstream &dst, const std::integral value)
		{
			if (std::isnan(value) || std::isinf(value))
			{
				dst << "null";
			}
			else
			{
				dst << value;
			}
		}

		static void write_number(std::ostringstream &dst, const std::floating_point value)
		{
			if (std::isnan(value) || std::isinf(value))
			{
				dst << "null";
			}
			else if (std::fabs(value) < 1.0e-300)
			{
				dst << "0";
			}
			else
			{
				dst << value;
			}
		}


		static void separator(std::ostringstream &dst, bool last)
		{
			if (!last)
			{
				dst << ",";
			}
		}

		static void object_start(std::ostringstream &dst, std::string_view name, std::stack<int> &stack)
		{
			write_name(dst, name, stack.size()) << '{';
		}

		static void object_end(std::ostringstream &dst, std::stack<int> &)
		{
			dst << '}';
		}

		static void array_start(std::ostringstream &dst, std::string_view name, std::stack<int> &stack)
		{
			write_name(dst, name, stack.size()) << '[';
		}

		static void array_end(std::ostringstream &dst, std::stack<int> &)
		{
			dst << ']';
		}

		static void item(std::ostringstream &dst, std::string_view name, int depth)
		{
			write_name(dst, name, depth) << "null";
		}

		static void item(std::ostringstream &dst, std::string_view name, bool value, int depth)
		{
			write_name(dst, name, depth) << (value ? "true" : "false");
		}

		static void item(std::ostringstream &dst, std::string_view name, int32_t value, int depth)
		{
			write_name(dst, name, depth);
			write_number(dst, value);
		}

		static void item(std::ostringstream &dst, std::string_view name, int64_t value, int depth)
		{
			write_name(dst, name, depth);
			write_number(dst, value);
		}

		static void item(std::ostringstream &dst, std::string_view name, double value, int depth)
		{
			write_name(dst, name, depth);
			write_number(dst, value);
		}

		static void item(std::ostringstream &dst, std::string_view name, std::span<uint8_t> value, int depth)
		{
			write_name(dst, name, depth) << '"' << base64::encode(value) << '"';
		}

		static void item(std::ostringstream &dst, std::string_view name, std::string_view value, int depth)
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
}


#endif


		// static const std::ios_base::openmode oflags = std::ios::out;

		// // Encoding functions
		// virtual string array_item_name([[maybe_unused]] int index) const 	{ return {}; }
		// virtual void separator([[maybe_unused]] os &dst, [[maybe_unused]] bool last) const	{}	// Item separator
		// virtual void object_start(os &dst, const string &name, stack<int> &stack) const = 0;
		// virtual void object_end(os &dst, stack<int> &stack) const = 0;
		// virtual void array_start(os &dst, const string &name, stack<int> &stack) const = 0;
		// virtual void array_end(os &dst, stack<int> &stack) const = 0;
		// virtual void item(os &dst, const string &name, int depth) const = 0;	// Array items have 0 length name
		// virtual void item(os &dst, const string &name, bool value, int depth) const = 0;
		// virtual void item(os &dst, const string &name, int32_t value, int depth) const = 0;
		// virtual void item(os &dst, const string &name, int64_t value, int depth) const = 0;
		// virtual void item(os &dst, const string &name, double value, int depth) const = 0;
		// virtual void item(os &dst, const string &name, const string &value, int depth) const = 0;
		// virtual void item(os &dst, const string &name, const vector<uint8_t> &value, int depth) const = 0;

		// // To avoid ambiguity and retain positive values cast unsigned integers to 64-bit longs
		// void item(os &dst, const string &name, uint32_t value, int depth) const { this->item(dst, name, (int64_t)value, depth); }


		// // Decoding functions
		// virtual bool validate(const string &data) const = 0;
		// virtual bool object_start(const string &data, int &i, int type) const = 0;
		// virtual bool object_end(const string &data, int &i) const = 0;
		// virtual bool item(const string &data, int &i, string &name, int &type) const = 0;
		// virtual bool array_start(const string &data, int &i, int type) const = 0;
		// virtual bool array_end(const string &data, int &i) const = 0;
		// virtual bool array_item(const string &data, int &i, int &type) const = 0;
		// virtual int skip(const string &data, int &i, int type) const = 0;

		// virtual bool get(const string &data, int &i, int type, bool def) const = 0;
		// virtual int32_t get(const string &data, int &i, int type, int32_t def) const = 0;
		// virtual int64_t get(const string &data, int &i, int type, int64_t def) const = 0;
		// virtual double get(const string &data, int &i, int type, double def) const = 0;
		// virtual string get(const string &data, int &i, int type, const string def) const = 0;
		// virtual vector<uint8_t> get(const string &data, int &i, int type, const vector<uint8_t> def) const = 0;

		// // peak whether or not the next value is null
		// virtual bool is_null(const string &data, int i, int type) const = 0;

		// // To avoid ambiguity and retain positive values cast unsigned integers to 64-bit longs
		// uint32_t get(const string &data, int &i, int type, uint32_t def) const { return this->get(data, i, type, (int64_t)def); }
