#pragma once

#include <entity/vref/vref.hpp>


namespace ent
{
	// Utility macros for invoking the appropriate reference constructor
	// If one argument is passed then string name of that argument is used as the mapping key
	// If two arguments are passed then the supplied name is used as the mapping key
	#ifndef ent_ref
		#define ent_get_ref(_1, _2, name, ...) name
		#define ent_man_ref(name, item)	std::make_pair(name, std::make_unique<ent::vref< \
			typename std::conditional<Constness, const std::remove_reference_t<decltype(item)>, std::remove_reference_t<decltype(item)>>::type \
		>>(item))

		#define ent_const(arg)			static constexpr bool Constness = arg
		#define ent_auto_ref(item)		ent_man_ref(#item, item)
		#define ent_ref(...)			ent_get_ref(__VA_ARGS__, ent_man_ref, ent_auto_ref, 0)(__VA_ARGS__)
		#define ent_map(...)			ent::mapping ent_describe() 		{ ent_const(false);	return { __VA_ARGS__ }; } \
		 								ent::mapping ent_describe() const	{ ent_const(true);	return { __VA_ARGS__ }; }
		#define ent_merge(base, ...)	ent::mapping ent_describe()			{ ent_const(false);	auto a = base::ent_describe(); a.insert({ __VA_ARGS__ }); return a; } \
										ent::mapping ent_describe() const	{ ent_const(true);	auto a = base::ent_describe(); a.insert({ __VA_ARGS__ }); return a; }

		// The following fails if an entity is declared inside a function due to the template not being permitted.
		// #define ent_functions			ent::mapping ent_describe()			{ return this->ent_impl<false>(); } \
		// 								ent::mapping ent_describe() const	{ return const_cast<std::add_pointer_t<std::decay_t<decltype(*this)>>>(this)->template ent_impl<true>(); }
		// #define ent_map(...)			template <bool Constness> ent::mapping ent_impl() { return { __VA_ARGS__ }; } ent_functions

		#if __cplusplus >= 202002L
			#define ent_single(...) __VA_ARGS__								// Wrap an argument containing commas so that it's not treated as multiple arguments to another macro
			#define ent_comma(item, ...) ent_auto_ref(item)__VA_OPT__(,)	// Add a comma if there are more items in the list

			#define ent_parens ()	// Note space before (), so object-like macro
			#define ent_expand(arg)  ent_expand1(ent_expand1(ent_expand1(ent_expand1(ent_single(arg)))))
			#define ent_expand1(arg) ent_expand2(ent_expand2(ent_expand2(ent_expand2(ent_single(arg)))))
			#define ent_expand2(arg) arg

			// The above already supports 28 entries - only enable this if a lot more are required
			// #define ent_expand2(arg) ent_expand3(ent_expand3(ent_expand3(ent_expand3(ent_single(arg)))))
			// #define ent_expand3(arg) arg

			#define ent_helper(macro, a1, ...)	macro(a1, __VA_ARGS__) __VA_OPT__(ent_again ent_parens (macro, __VA_ARGS__))
			#define ent_for_each(macro, ...)	__VA_OPT__(ent_expand(ent_helper(macro, __VA_ARGS__)) )
			#define ent_again()					ent_helper
			#define ent_map_terse(...)			ent::mapping ent_describe()       { ent_const(false); return { ent_for_each(ent_comma, __VA_ARGS__) }; } \
												ent::mapping ent_describe() const { ent_const(true);  return { ent_for_each(ent_comma, __VA_ARGS__) }; }

			#define edesc ent_map_terse
		#endif

		// Concise call to ent_ref and ent_map, disable if they conflict
		#define eref ent_ref
		#define emap ent_map
		#define emerge ent_merge
	#endif


	// Encode an entity
	template <class Codec, class T> std::string encode(const T &item)
	{
		// static_assert(!std::is_const<T>::value, "Cannot encode a const entity");
		static_assert(std::is_base_of<codec, Codec>::value,	"Invalid codec specified");

		stack<int> stack;
		os result(Codec::oflags);

		vref<const T>::encode(item, Codec(), result, "", stack);

		return result.str();
	}


	// Decode an entity
	template <class Codec, class T> T decode(const std::string &data, T &item, bool skipValidation = false)
	{
		static_assert(!std::is_const<T>::value, "Cannot decode to a const entity");
		static_assert(std::is_base_of<codec, Codec>::value,	"Invalid codec specified");

		Codec c;

		if (skipValidation || c.validate(data))
		{
			vref<T>::decode(item, c, data, 0, -1);
		}

		return item;
	}


	// Decode and create an entity
	template <class Codec, class T> T decode(const std::string &data, bool skipValidation = false)
	{
		static_assert(!std::is_const<T>::value, "Cannot decode to a const entity");
		static_assert(std::is_base_of<codec, Codec>::value,	"Invalid codec specified");

		T result;
		return decode<Codec>(data, result, skipValidation);
	}


	// Encode a tree
	template <class Codec> static std::string encode(const tree &item)
	{
		static_assert(std::is_base_of<codec, Codec>::value,	"Invalid codec specified");

		os result(Codec::oflags);
		stack<int> stack;

		if (item.get_type() == tree::Type::Object || item.get_type() == tree::Type::Array)
		{
			Codec().item(item, result, "", stack);
		}

		return result.str();
	}


	// Decode to a tree
	template <class Codec> static tree decode(const std::string &data, bool skipValidation = false)
	{
		static_assert(std::is_base_of<codec, Codec>::value,	"Invalid codec specified");

		Codec c;
		int position = 0;

		if (skipValidation || c.validate(data))
		{
			return c.is_object(data) ? c.object(data, position, -1) : c.array(data, position, -1);
		}

		return {};
	}


	// Convert entities to/from a tree
	template <class T> static tree to_tree(const T &item)				{ return vref<const T>::to_tree(item); }
	template <class T> static T &from_tree(const tree &data, T &item)	{ vref<T>::from_tree(item, data);	return item; }
	template <class T> static T from_tree(const tree &data)				{ T result; 						return from_tree(data, result); }
}
