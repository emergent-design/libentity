#pragma once

#include <entity/experimental/treeview.hpp>
#include <charconv>
// #include <ranges>
#include <bitset>


// #include <iostream>


namespace ent::experimental
{
	using std::string_view;

	// parser - avoid exceptions!!
	// separate parser and generator into sub structures?
	struct json
	{
		using Type = treeview::Type;


		// Whitespace characters (tab, new line, carriage return, space, comma)
		static constexpr bool whitespace[256] = {
			0,0,0,0,0,0,0,0,0,1, //   0 \t
			1,0,0,1,0,0,0,0,0,0, //  10 \n \r
			0,0,0,0,0,0,0,0,0,0, //  20
			0,0,1,0,0,0,0,0,0,0, //  30 space
			0,0,0,0,1,0,0,0,0,0, //  40 ,
		};

		// whitespace and terminators
		static constexpr bool terminators[256] = {
			0,0,0,0,0,0,0,0,0,1, //   0 \t
			1,0,0,1,0,0,0,0,0,0, //  10 \n \r
			0,0,0,0,0,0,0,0,0,0, //  20
			0,0,1,0,0,0,0,0,0,0, //  30 space
			0,0,0,0,1,0,0,1,0,0, //  40 , /
			0,0,0,0,0,0,0,0,0,0, //  50
			0,0,0,0,0,0,0,0,0,0, //  60
			0,0,0,0,0,0,0,0,0,0, //  70
			0,0,0,0,0,0,0,0,0,0, //  80
			0,0,0,1,0,0,0,0,0,0, //  90 ]
			0,0,0,0,0,0,0,0,0,0, // 100
			0,0,0,0,0,0,0,0,0,0, // 110
			0,0,0,0,0,1,0,0,0,0, // 120 }
		};


		static treeview decode(const string_view data)
		{
			// validate?

			auto trimmed = skip_whitespace(data);

			if (trimmed.starts_with('{'))
			{
				return object(trimmed).second;
			}
			if (trimmed.starts_with('['))
			{
				return array(trimmed).second;
			}

			// error - invalid JSON
			return {};
		}


		template <typename T> static inline constexpr T number(const std::string_view s)
		{
			T value {};

			// #if __cpp_lib_to_chars >= 202306L
			// 	if (std::from_chars(s.data(), s.data() + s.size(), value, 0))
			// #else

			if constexpr (std::is_integral_v<T>)
			{
				if (s.substr(0, 2) == "0x") // || s.substr(0, 2) == "0X"
				{
					if (std::from_chars(s.data() + 2, s.data() + s.size(), value, 16).ec == std::errc{})
					{
						return value;
					}
				}
				else if (s.substr(0, 3) == "-0x")
				{
					if (std::from_chars(s.data() + 3, s.data() + s.size(), value, 16).ec == std::errc{})
					{
						return -value;
					}
				}
				else if (std::from_chars(s.data(), s.data() + s.size(), value).ec == std::errc{})
				{
					return value;
				}
			}
			else if (std::from_chars(s.data(), s.data() + s.size(), value).ec == std::errc{})
			{
				return value;
			}

			// error - value is not a valid number??
			// error("value is not a valid number", data, i - s.size());

			return {};
		}


		static std::string escape(const std::string_view s)
		{
			std::string result;
			size_t current	= 0;
			size_t next		= s.find('\\', 0);

			result.reserve(s.size());

			while (next != std::string_view::npos)
			{
				result.append(s.substr(current, next - current));

				if (next < s.size() - 1)
				{
					switch (s[next+1])
					{
						case '"':	result.append("\"");	break;
						case '\\':	result.append("\\");	break;
						case 't':	result.append("\t");	break;
						case 'n':	result.append("\n");	break;
						case 'r':	result.append("\r");	break;
						case 'b':	result.append("\b");	break;
						case 'f':	result.append("\f");	break;
						case 'u':	result.append("\\u");	break;	// Unicode characters are just passed straight through
					}
				}

				current	= next + 1;
				next	= s.find('\\', current);
			}

			return result.append(s.substr(current));

			// bool special = false;
		// 	for (auto &c : s)
		// 	{
		// 		if (special)
		// 		{
		// 			switch (c)
		// 			{
		// 				case '"':	result.append("\"");	break;
		// 				case '\\':	result.append("\\");	break;
		// 				case 't':	result.append("\t");	break;
		// 				case 'n':	result.append("\n");	break;
		// 				case 'r':	result.append("\r");	break;
		// 				case 'b':	result.append("\b");	break;
		// 				case 'f':	result.append("\f");	break;
		// 				case 'u':	result.append("\\u");	break;	// Unicode characters are just passed straight through
		// 			}
		// 		}
		// 		else if (c == '\\')
		// 		{
		// 			special = true;
		// 		}
		// 		else
		// 		{
		// 			result += c;
		// 		}
		// 	}

		// 	return result;
		}


		static ent::tree translate(const treeview &item)
		{
			if (item.type == Type::String)
			{
				// // if (item.leaf.contains('\\'))
				// if (item.leaf.find('\\') != std::string_view::npos)
				// {
				// 	// Handle special characters in the string
				// 	return escape(item.leaf);
				// }
				// // Otherwise just return it
				// return item.leaf;

				return escape(item.leaf);
			}

			if (item.type == Type::Simple)
			{
				if (item.leaf == "true")	return true;
				if (item.leaf == "false")	return false;
				if (item.leaf == "null")	return nullptr;

				if (item.leaf.find('.') == std::string_view::npos)
				{
					return number<int64_t>(item.leaf);
				}
				else
				{
					return number<double>(item.leaf);
				}
			}

			return {};
		}


		static string_view skip_whitespace(const string_view data)
		{
			for (size_t i=0; i<data.length(); i++)
			{
				if (!whitespace[(uint8_t)data[i]])
				{
					if (data[i] == '/')
					{
						i += 1 + skip_comment(data.substr(i + 1));
					}
					else
					{
						return data.substr(i);
					}
				}
			}

			return {};
		}


		static size_t skip_comment(const string_view data)
		{
			if (data.starts_with('/'))
			{
				// Single line comment
				for (size_t i=0; i<data.length(); i++)
				{
					if (data[i] == '\n' || data[i] == '\r')
					{
						return i;
					}
				}
			}
			else if (data.starts_with('*'))
			{
				// Block comment
				for (size_t i=0; i<data.length()-1; i++)
				{
					if (data[i] == '*' && data[i+1] == '/')
					{
						return i + 1;
					}
				}
			}

			// error - invalid comment type
			return data.length();
		}


		static std::pair<string_view, string_view> key(const string_view data)
		{
			for (size_t i=1; i<data.length(); i++)
			{
				if (data[i] == '"')
				{
					return {
						data.substr(i + 1),
						data.substr(1, i - 1)
					};
				}
			}

			// error - unterminated key
			return {};
		}


		static std::pair<string_view, std::optional<treeview>> string(const string_view data)
		{
			for (size_t i=1; i<data.length(); i++)
			{
				if (data[i] == '"')
				{
					return {
						data.substr(i + 1),
						treeview { .type = treeview::Type::String, .leaf = data.substr(1, i - 1) }
					};
				}
				if (data[i] == '\\')
				{
					i++;	// Skip the next escaped character
				}
			}

			// error - unterminated string
			return {};
		}


		// Extract a simple value (null, boolean, numeric)
		static std::pair<string_view, std::optional<treeview>> simple(const string_view data)
		{
			for (size_t i=0; i<data.length(); i++)
			{
				if (terminators[(uint8_t)data[i]])
				{
					return {
						data.substr(i),
						treeview { .type = Type::Simple, .leaf = data.substr(0, i) }
					};
				}
			}

			return {};
		}


		static std::pair<string_view, std::optional<treeview>> item(const string_view data)
		{
			if (data.starts_with('{')) return object(data);
			if (data.starts_with('[')) return array(data);
			if (data.starts_with('"')) return string(data);
			if (data.starts_with('}'))
			{
				// error - missing object value
				return {};
			}

			return simple(data);
		}


		static std::pair<string_view, treeview> object(const string_view data)
		{
			// treeview result {};
			// treeview result { .children = std::make_shared<std::map<std::string_view, treeview>>() };
			// treeview result { .type = Type::Array, .items = std::make_shared<std::vector<treeview>>() };
			auto result = treeview::object();

			auto current = skip_whitespace(data.substr(1));	// first character should be {

			while (!current.empty())
			{
				// std::cout << current << '\n';
				if (current.starts_with('}'))
				{
					return { current.substr(1), result };
				}

				if (current.starts_with('"'))
				{
					auto [updated, k] = key(current);

					if (k.empty())
					{
						// error - empty or unterminated key
						return {};
					}

					current = skip_whitespace(updated);

					if (!current.starts_with(':'))
					{
						// error - missing key/value separator
						return {};
					}

					current = skip_whitespace(current.substr(1));

					// std::cout << " after key == " << current << '\n';

					auto [again, value] = item(current);

					current = skip_whitespace(again);

					// std::cout << " after value == " << again << '\n';

					if (value)
					{
						result.children->emplace(k, value.value());
						// result.items->emplace_back(value.value().with_key(k));
						// result.children[k] = value.value();
						// result.items.push_back(value.value().with_key(k));
					}
					else
					{
						// error - missing object value
						return {};
					}
				}
				else
				{
					// error missing key
					return {};
				}
			}

			// error - failed to find end of object

			return {};
		}


		static std::pair<string_view, treeview> array(const string_view data)
		{
			// treeview result { .type = Type::Array, .items = std::make_shared<std::vector<treeview>>() };
			auto result = treeview::array();

			auto current = skip_whitespace(data.substr(1));	// first character should be [

			while (!current.empty())
			{
				if (current.starts_with(']'))
				{
					return { current.substr(1), result };
				}

				auto [again, value] = item(current);

				current = skip_whitespace(again);

				if (value)
				{
					// result.items.push_back(value.value());
					result.items->push_back(value.value());
				}
				else
				{
					// error - invalid array item
					return {};
				}
			}

			return {};
		}



	};
}
