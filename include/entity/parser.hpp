#pragma once

//#include <entity/treemap.hpp>
#include <entity/entity.hpp>
#include <entity/tree.hpp>


namespace ent
{
	/*template <class Codec, class T> static std::string encode(T &item)
	{
		static_assert(std::is_base_of<entity2, T>::value,	"Item must be derived from entity");
		static_assert(std::is_base_of<codec, Codec>::value,	"Invalid codec specified");

		os result(Codec::oflags);
		std::stack<int> stack;

		vref<T>::encode(item, Codec(), result, "", stack);

		return result.str();
	}


	template <class Codec, class T> static T decode(const std::string &data, bool skipValidation = false)
	{
		T result;
		return decode<Codec>(data, result, skipValidation);
	}


	template <class Codec, class T> static T &decode(const std::string &data, T &item, bool skipValidation = false)
	{
		static_assert(std::is_base_of<entity2, T>::value,	"Item must be derived from entity");
		static_assert(std::is_base_of<codec, Codec>::value,	"Invalid codec specified");

		Codec c;

		if (skipValidation || c.validate(data))
		{
			vref<T>::decode(item, c, data, 0, -1);
		}

		return item;
	}*/


	/*template <class Codec> static std::string encode_tree(const tree2 &item)
	{
		static_assert(std::is_base_of<codec, Codec>::value,	"Invalid codec specified");

		os result(Codec::oflags);
		stack<int> stack;

		if (item.get_type() == tree2::Type::Object)
		{
			Codec().item(item, result, "", stack);
		}

		return result.str();
	}


	template <class Codec> static tree2 decode_tree(const std::string &data, bool skipValidation = false)
	{
		static_assert(std::is_base_of<codec, Codec>::value,	"Invalid codec specified");

		Codec c;
		int position = 0;

		if (skipValidation || c.validate(data))
		{
			return c.object(data, position, -1);
		}

		return {};
	}*/
}

