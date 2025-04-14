#pragma once

#include <entity/tree.hpp>
#include <optional>
#include <string>
#include <vector>
#include <memory>
#include <map>


namespace ent::experimental
{
	// The fundamental types that can be stored in the view structure. These
	// mirror those that are supported by JSON with an additional special
	// case for binary data.
	enum class view_type : uint8_t
	{
		Null, String, Integer, Floating, Boolean, Binary, Array, Object,

		// Simple // this is reserved for values that are figured out when requested such as null, bools, and numerics
		Unknown
	};


	template <typename Decoder, typename T> struct view
	{
		using decoder = Decoder;
		view_type type = view_type::Object;

		T leaf;

		std::shared_ptr<std::vector<view>> items;
		std::shared_ptr<std::map<std::string_view, view>> children;

		// What if the parser accepted a shared_ptr to string data. Then copy it
		// into the root treeview, which will guarantee it exists for the lifetime
		// of the view?

		// view() = default;
		// view(const T &&leaf) : leaf(std::move(leaf)) {}


		template <typename U> std::optional<U> value()
		{
			if (this->type != view_type::Array && this->type != view_type::Object)
			{
				return decoder::template translate<U>(this->leaf);
			}

			return std::nullopt;
			// if constexpr (std::is_integral_v<U>)
			// {
			// 	if (this->type == view_type::Integer || this->type == view_type::Simple)
			// 	{
			// 		return decoder::value<U>(this->leaf);
			// 	}
			// }
		}


		static view array()
		{
			return {
				.type = view_type::Array,
				.items = std::make_shared<std::vector<view>>()
			};
		}

		static view object()
		{
			return {
				.type = view_type::Object,
				.children = std::make_shared<std::map<std::string_view, view>>()
			};
		}

		static view object(std::initializer_list<typename std::map<std::string_view, view>::value_type> items)
		{
			return {
				.type = view_type::Object,
				.children = std::make_shared<std::map<std::string_view, view>>(items)
			};
		}
	};


	template <typename T, typename View> ent::tree get_value(const View &view)
	{
		T value = {};

		if (View::decoder::translate(view, value))
		{
			return value;
		}

		return {};
	}

	template <typename View> ent::tree to_tree(const View &view)
	{
		using T = view_type;

		switch (view.type)
		{
			case T::Null:		return nullptr;
			case T::Binary: 	return nullptr;	// Not sure about binary at this point
			case T::Integer:	return get_value<int64_t>(view);
			case T::Floating:	return get_value<double>(view);
			case T::Boolean:	return get_value<bool>(view);
			case T::String:		return get_value<std::string>(view);
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
				result.push_back(to_tree(v));
			}

			return result;
		}

		if (view.type == T::Object && view.children)
		// if (view.type == T::Object && view.items)
		{
			ent::tree result;

			for (auto &[k, v] : *view.children)
			{
				result.set(std::string(k), to_tree(v));
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

		// return Decoder::translate(view);
		return {};

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

