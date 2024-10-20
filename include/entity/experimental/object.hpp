#pragma once

#include <string>
// #include <entity/vref/vref.hpp>
#include <memory>
#include <vector>
#include <map>


namespace ent::experimental
{
	// template <typename T> using if_simple = typename std::enable_if<
	// 	   std::is_arithmetic_v<T>
	// 	|| std::is_same_v<string, std::remove_const_t<T>>
	// 	|| std::is_same_v<vector<uint8_t>, std::remove_const_t<T>>
	// >::type;


	// Member accessor base class
	struct member
	{
		// get();
		// set();
	};

	template <std::integral T> struct member_integer : member
	{
		member_integer(T &reference) : reference(&reference) {}


		T *reference;
	};


	// lightweight view into an entity or ent::tree?
	struct objectview
	{
		enum class Type : uint8_t
		{
			Null, String, Integer, Floating, Boolean, Binary, Array, Object
		};


		Type type = Type::Object;

		// std::shared_ptr<vbase> leaf;
		// accessor leaf??

		std::shared_ptr<std::vector<objectview>> items;
		std::shared_ptr<std::map<std::string_view, objectview>> children;

		static objectview array()
		{
			return {
				.type = Type::Array,
				.items = std::make_shared<std::vector<objectview>>()
			};
		}

		static objectview object()
		{
			return {
				.type = Type::Object,
				.children = std::make_shared<std::map<std::string_view, objectview>>()
			};
		}
	};
}

// typedef std::map<std::string, std::shared_ptr<vbase>> mapping;
