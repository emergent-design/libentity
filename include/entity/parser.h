#pragma once

#include <entity/entity2.h>


namespace ent
{
	template <class C, class T> static std::string encode(T &item)
	{
		static_assert(std::is_base_of<entity2, T>::value,	"Item must be derived from entity");
		static_assert(std::is_base_of<codec, C>::value,		"Invalid codec specified");

		os result(C::oflags);
		std::stack<int> stack;

		vref<T>::encode(item, C(), result, "", stack);

		return result.str();
	}


	template <class C, class T> static T decode(const std::string &data)
	{
		T result;
		return decode<C>(data, result);
	}


	template <class C, class T> static T &decode(const std::string &data, T &item)
	{
		static_assert(std::is_base_of<entity2, T>::value,	"Item must be derived from entity");
		static_assert(std::is_base_of<codec, C>::value,		"Invalid codec specified");

		//std::stack<int> stack;
		if (C().validate(data))
		{
			vref<T>::decode(item, C(), data, 0, 0);
		}

		return item;
	}

}

