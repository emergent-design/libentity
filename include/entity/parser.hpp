#pragma once

#include <entity/treemap.hpp>
#include <entity/entity2.h>


namespace ent
{
	template <class Codec, class T> static std::string encode(T &item)
	{
		static_assert(std::is_base_of<entity2, T>::value,	"Item must be derived from entity");
		static_assert(std::is_base_of<codec, Codec>::value,	"Invalid codec specified");

		os result(Codec::oflags);
		std::stack<int> stack;

		vref<T>::encode(item, Codec(), result, "", stack);

		return result.str();
	}


	template <class Codec, class T> static T decode(const std::string &data)
	{
		T result;
		return decode<Codec>(data, result);
	}


	template <class Codec, class T> static T &decode(const std::string &data, T &item, bool skipValidation = false)
	{
		static_assert(std::is_base_of<entity2, T>::value,	"Item must be derived from entity");
		static_assert(std::is_base_of<codec, Codec>::value,	"Invalid codec specified");

		Codec codec;

		if (skipValidation || codec.validate(data))
		{
			vref<T>::decode(item, codec, data, 0, -1);
		}

		return item;
	}


	template <class Codec> static std::string encode_tree(const tree2 &item)
	{
		static_assert(std::is_base_of<codec, Codec>::value,	"Invalid codec specified");

		os result(Codec::oflags);
		std::stack<int> stack;

		if (item.get_type() == tree2::Type::Object)
		{
			treemap::encode(item, Codec(), result, "", stack);
		}

		return result.str();
	}


	template <class Codec> static tree2 decode_tree(const std::string &data, bool skipValidation = false)
	{
		static_assert(std::is_base_of<codec, Codec>::value,	"Invalid codec specified");

		tree2 result;
		Codec codec;

		if (skipValidation || codec.validate(data))
		{
			treemap::decode(result, codec, data, 0, -1);
		}

		return result;
	}
}

