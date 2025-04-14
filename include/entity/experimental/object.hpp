#pragma once

#include <entity/experimental/view.hpp>
#include <string>
// #include <entity/vref/vref.hpp>
#include <memory>
#include <vector>
#include <map>

//debug
#include <iostream>

using namespace std::literals::string_view_literals;


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

	using Member = std::shared_ptr<member>;


	struct member_decoder
	{
		template <typename T> static bool translate(const view<member_decoder, Member> &src, T &dst)
		{
			return false;
		}
	};

	// typename std::enable_if<
	// 	   std::is_arithmetic_v<T>
	// 	|| std::is_same_v<string, std::remove_const_t<T>>
	// 	|| std::is_same_v<vector<uint8_t>, std::remove_const_t<T>>
	// >::type;


	using MemberView = view<member_decoder, Member>;

	template <typename T> concept Simple = std::is_arithmetic_v<T> || std::is_same_v<string, std::remove_const_t<T>>;

	template <Simple T> struct member_ref : member
	{
		member_ref(const T &reference) : reference(&reference) {}


		static constexpr view_type get_type()
		{
			if constexpr (std::is_same_v<bool, T>)
			{
				return view_type::Boolean;
			}
			if constexpr (std::is_same_v<string, std::remove_const_t<T>>)
			{
				return view_type::String;
			}
			if constexpr (std::is_integral_v<T>)
			{
				return view_type::Integer;
			}
			if constexpr (std::is_floating_point_v<T>)
			{
				return view_type::Floating;
			}

			return view_type::Unknown;
		}

		static MemberView to_view(const T &reference)
		{
			return {
				.type = get_type(),
				.leaf = std::make_shared<member_ref>(reference)
			};
		}

		template <typename View> static bool from_view(const View &view, T &reference)
		{
			return View::decoder::translate(view, reference);
			// if constexpr (std::is_same_v<string, std::remove_const_t<T>>)
			// {
			// 	if (view.type == view_type::String)
			// 	{
			// 		reference = View::decoder::string(view.leaf)
			// 	}
			// }
		}


		const T *reference;
	};


	// template <typename T> std::pair<std::string_view, MemberView> to_child_view(std::string_view name, const T &reference)
	// {
	// 	return std::make_pair(
	// 		name,
	// 		member_ref<const std::remove_reference_t<T>>::to_view(reference)
	// 	);
	// }
	template <typename T> inline auto to_child_view(std::string_view name, const T &reference)
	{
		return std::make_pair(
			name,
			member_ref<const std::remove_reference_t<T>>::to_view(reference)
		);
	}

	template <typename View, typename T> inline bool from_child_view(const View &view, std::string_view name, T &dst)
	{
		// std::cout << int(view.children->at(name).type) << '\n';
		return view.children->contains(name)
			&& member_ref<T>::from_view(view.children->at(name), dst);
			// && member_ref<const std::remove_reference_t<T>>::from_view(view.children->at(name), dst);
			// && View::decoder::translate(view.children->at(name), dst);
	}



	struct SimpleEntity
	{
		std::string name	= "simple";
		bool flag			= true;
		int integer			= 42;
		int64_t bignumber	= 20349758;
		double floating		= 3.142;


		const MemberView to_view() const
		{
			return MemberView::object({
				to_child_view("name", name),
				to_child_view("flag", flag),
				to_child_view("integer", integer),
				to_child_view("bignumber", bignumber),
				to_child_view("floating", floating)
			});
			// return {
			// 	.type 		= view_type::Object,
			// 	.children	= std::make_shared<std::map<std::string_view, MemberView>>({ //std::initializer_list<std::map<std::string_view, MemberView>::value_type>{
			// 		to_child_view("name", name)
			// 		// member_ref<const std::remove_reference_t<decltype(name)>>::to_view("name", name)
			// 		// { "name", member_ref<const std::remove_reference_t<decltype(name)>>::to_view(name) }

			// 		// std::make_pair<std::string_view, MemberView>("name", std::make_unique<member_ref<const std::remove_reference_t<decltype(name)>>>(name))
			// 		// { "name", std::make_unique<member_ref<const std::remove_reference_t<decltype(name)>>>(name)
			// 	})
			// };
		}

		template <typename View> bool from_view(const View &view)
		{
			if (view.type != view_type::Object || !view.children)
			{
				return false;
			}


			from_child_view(view, "name", name);
			from_child_view(view, "flag", flag);
			from_child_view(view, "integer", integer);
			from_child_view(view, "bignumber", bignumber);
			from_child_view(view, "floating", floating);

			return true;
		}

	};

		// #define ent_man_ref(name, item)	std::make_pair(name, std::make_unique<ent::vref< \
	// 		typename std::conditional<Constness, const std::remove_reference_t<decltype(item)>, std::remove_reference_t<decltype(item)>>::type \
	// 	>>(item))

	// struct ComplexEntity
	// {
	// 	string name;
	// 	vector<SimpleEntity> entities;
	// 	array<SimpleEntity, 4> extras;
	// 	CollectionEntity collection;
	// 	SimpleEntity simple;

	// 	emap(eref(name), eref(entities), eref(extras), eref(collection), eref(simple))
	// };

	// struct TreeEntity
	// {
	// 	string name;
	// 	tree parameters;

	// 	emap(eref(name), eref(parameters))
	// };







	// // lightweight view into an entity or ent::tree?
	// struct objectview
	// {
	// 	enum class Type : uint8_t
	// 	{
	// 		Null, String, Integer, Floating, Boolean, Binary, Array, Object
	// 	};


	// 	Type type = Type::Object;

	// 	// std::shared_ptr<vbase> leaf;
	// 	// accessor leaf??

	// 	std::shared_ptr<std::vector<objectview>> items;
	// 	std::shared_ptr<std::map<std::string_view, objectview>> children;

	// 	static objectview array()
	// 	{
	// 		return {
	// 			.type = Type::Array,
	// 			.items = std::make_shared<std::vector<objectview>>()
	// 		};
	// 	}

	// 	static objectview object()
	// 	{
	// 		return {
	// 			.type = Type::Object,
	// 			.children = std::make_shared<std::map<std::string_view, objectview>>()
	// 		};
	// 	}
	// };
}

// typedef std::map<std::string, std::shared_ptr<vbase>> mapping;
