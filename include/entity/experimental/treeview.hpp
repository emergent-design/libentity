#pragma once

#include <entity/tree.hpp>
#include <ranges>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <variant>

namespace ent::experimental
{
	// A treeview is a lightweight view into a blob of encoded data.
	// The codec must still be used to convert values from the encoded data in the appropriate way.
	// The source data must still exist when this treeview is accessed.
	struct treeview
	{
		// The fundamental types that can be stored in the treeview structure. These
		// mirror those that are supported by JSON with an additional special
		// case for binary data.
		enum class Type : uint8_t
		{
			Null, String, Integer, Floating, Boolean, Binary, Array, Object,

			Simple // this is reserved for values that are figured out when requested such as null, bools, and numerics
		};


		Type type = Type::Object;

		// std::variant<
		// 	std::string_view,
		// 	std::shared_ptr<std::vector<treeview>>,
		// 	std::shared_ptr<std::map<std::string_view, treeview>>
		// > data;

		// union
		// {
		// 	std::string_view leaf;
		// 	std::shared_ptr<std::vector<treeview>> items;
		// 	std::shared_ptr<std::map<std::string_view, treeview>> children;
		// };



		// std::string_view key;	// The key for this node if it has one
		std::string_view leaf;

		// // // std::map<std::string_view, treeview> children;
		// // // std::unordered_map<std::string_view, treeview> children;
		// // // std::vector<treeview> items;
		// // // std::list<treeview> items;
		std::shared_ptr<std::vector<treeview>> items;
		std::shared_ptr<std::map<std::string_view, treeview>> children;


		// What if the parser accepted a shared_ptr to string data. Then copy it
		// into the root treeview, which will guarantee it exists for the lifetime
		// of the view?


		// treeview &with_key(std::string_view key)
		// {
		// 	this->key = key;
		// 	return *this;
		// }

		static treeview array()
		{
			return {
				.type = Type::Array,
				.items = std::make_shared<std::vector<treeview>>()
			};
		}

		static treeview object()
		{
			return {
				.type = Type::Object,
				.children = std::make_shared<std::map<std::string_view, treeview>>()
			};
		}
	};

	/*template <typename Codec> struct parsed_view
	{
		// The fundamental types that can be stored in the treeview structure. These
		// mirror those that are supported by JSON with an additional special
		// case for binary data.
		enum class Type : uint8_t
		{
			Null, String, Integer, Floating, Boolean, Binary, Array, Object,

			Simple // this is reserved for values that are figured out when requested such as null, bools, and numerics (JSON)
		};


		Type type = Type::Object;

		std::string_view key;	// The key for this node if it has one
		std::string_view leaf;

		// std::map<std::string_view, treeview> children;
		// std::vector<treeview> items;
		// std::list<treeview> items;
		// std::shared_ptr<std::map<std::string_view, treeview>> children;

		std::shared_ptr<std::vector<parsed_view>> items;
		std::shared_ptr<std::map<std::string_view, parsed_view>> children;


		treeview &with_key(std::string_view key)
		{
			this->key = key;
			return *this;
		}



		ent::tree to_tree()
		{
			if (this->type == Type::Null || this->type == Type::Binary) // Not sure about binary at this point
			{
				return nullptr;
			}

			if (this->type == Type::Array && this->items)
			{
				std::vector<ent::tree> result;

				for (auto &v : *this->items)
				{
					result.push_back(to_tree(v));
				}

				return result;
			}

			if (this->type == Type::Object && this->children)
			{
				ent::tree result;

				for (auto &[k, v] : *this->children)
				{
					result.set(std::string(k), to_tree<Codec>(v));
				}
				// for (auto &v : this->items)
				// {
				// 	result.set(std::string(v.key), to_tree(v));
				// }

				return result;
			}

			return Codec::translate(*this);
		}
	};
*/

	template <typename Codec> ent::tree to_tree(const treeview &view)
	{
		using T = treeview::Type;

		if (view.type == T::Null || view.type == T::Binary) // Not sure about binary at this point
		{
			return nullptr;
		}

		if (view.type == T::Array && view.items)
		{
			std::vector<ent::tree> result;

			// for (auto &v : view.items)
			// {
			// 	result.push_back(to_tree<Codec>(v));
			// }
			for (auto &v : *view.items)
			{
				result.push_back(to_tree<Codec>(v));
			}

			return result;
		}

		if (view.type == T::Object && view.children)
		// if (view.type == T::Object && view.items)
		{
			ent::tree result;

			for (auto &[k, v] : *view.children)
			{
				result.set(std::string(k), to_tree<Codec>(v));
			}

			// for (auto &v : *view.items)
			// {
			// 	result.set(std::string(v.key), to_tree<Codec>(v));
			// }
			// for (auto &v : view.items)
			// {
			// 	result.set(std::string(v.key), to_tree<Codec>(v));
			// }

			return result;
		}

		return Codec::translate(view);

		// switch (view.type)
		// {
		// 	case T::Null:		return nullptr;
		// 	case T::Binary:		return nullptr;	// ??
		// 	// case T::String:		return view.leaf;
		// 	// case T::Integer:	return Codec::translate(view);
		// 	// case T::Floating:	return Codec::translate(view);
		// 	// case T::Boolean:	return Codec::translate(view);
		// 	// case T::Simple:		return Codec::translate(view);

		// 	// case T::Array:		return std::ranges::transform_view(view.items, [](auto &i) { return to_tree<Codec>(i); }) | std::ranges::to_vector();
		// 	case T::Array: {
		// 		std::vector<ent::tree> result;

		// 		for (auto &v : view.items)
		// 		{
		// 			result.push_back(to_tree<Codec>(v));
		// 		}

		// 		return result;
		// 	}
		// 	case T::Object: {

		// 		ent::tree result;

		// 		// for (auto &[k, v] : view.children)
		// 		// {
		// 		// 	result.set(std::string(k), to_tree<Codec>(v));
		// 		// }
		// 		for (auto &v : view.items)
		// 		{
		// 			result.set(std::string(v.key), to_tree<Codec>(v));
		// 		}

		// 		return result;
		// 	}

		// 	default: return Codec::translate(view);
		// }
	}



}
