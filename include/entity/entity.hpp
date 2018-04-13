#pragma once

#include <entity/vref.hpp>


namespace ent
{
	// Utility macros for invoking the appropriate reference constructor
	// If one argument is passed then string name of that argument is used as the mapping key
	// If two arguments are passed then the supplied name is used as the mapping key
	#ifndef ent_ref
		#define ent_get_ref(_1, _2, name, ...) name
		#define ent_man_ref(name, item)		std::make_pair(name, std::make_shared<ent::vref<typename std::remove_reference<decltype(item)>::type>>(item))
		#define ent_auto_ref(item)			ent_man_ref(#item, item)
		#define ent_ref(...)				ent_get_ref(__VA_ARGS__, ent_man_ref, ent_auto_ref)(__VA_ARGS__)
		#define ent_map(...)				ent::mapping ent_describe() { return { __VA_ARGS__ }; }
		#define ent_merge(base, ...)		ent::mapping ent_describe() { auto a = base::ent_describe(); a.insert({ __VA_ARGS__ });	return a; }

		// Concise call to ent_ref and ent_map, disable if they conflict
		#define eref ent_ref
		#define emap ent_map
		#define emerge ent_merge
	#endif



	// Encode an entity
	template <class Codec, class T> std::string encode(T &item)
	{
		static_assert(!std::is_const<T>::value, "Cannot encode a const entity");
		static_assert(std::is_base_of<codec, Codec>::value,	"Invalid codec specified");

		stack<int> stack;
		os result(Codec::oflags);

		vref<T>::encode(item, Codec(), result, "", stack);

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
	template <class T> static tree to_tree(T &item)						{ return vref<T>::to_tree(item); }
	template <class T> static T &from_tree(const tree &data, T &item)	{ vref<T>::from_tree(item, data);	return item; }
	template <class T> static T from_tree(const tree &data)				{ T result; 						return from_tree(data, result); }
}
