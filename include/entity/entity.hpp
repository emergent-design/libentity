#pragma once

#include <entity/vref.hpp>


namespace ent
{
	// Utility macros for invoking the appropriate reference constructor
	// If one argument is passed then string name of that argument is used as the mapping key
	// If two arguments are passed then the supplied name is used as the mapping key
	#ifndef ent_ref
		#define ent_get_ref(_1, _2, name, ...) name
		#define ent_man_ref(name, item)		std::make_pair(name, std::make_shared<ent::vref<decltype(item)>>(item))
		#define ent_auto_ref(item)			ent_man_ref(#item, item)
		#define ent_ref(...)				ent_get_ref(__VA_ARGS__, ent_man_ref, ent_auto_ref)(__VA_ARGS__)

		// Concise call to ent_ref, disable if it conflicts and use ent_ref instead
		#define eref ent_ref
	#endif

	typedef std::map<std::string, std::shared_ptr<vbase>> mapping;


	// The abstract base class for serialisable objects.
	// Inherit from this, implement the describe function correctly,
	// and your class will become serialisable.
	class entity
	{
		template <class T, class U> friend struct vref;

		public:

			template <class Codec, class T> static std::string encode(T &item)
			{
				static_assert(std::is_base_of<entity, T>::value,	"Item must be derived from entity");
				static_assert(std::is_base_of<codec, Codec>::value,	"Invalid codec specified");

				os result(Codec::oflags);
				std::stack<int> stack;

				vref<T>::encode(item, Codec(), result, "", stack);

				return result.str();
			}

			template <class Codec, class T> static std::string encode(std::vector<T> &items)
			{
				static_assert(std::is_base_of<entity, T>::value,	"Item must be derived from entity");
				static_assert(std::is_base_of<codec, Codec>::value,	"Invalid codec specified");

				os result(Codec::oflags);
				std::stack<int> stack;

				vref<std::vector<T>>::encode(items, Codec(), result, "", stack);

				return result.str();
			}


			template <class Codec, class T> static T decode(const std::string &data, bool skipValidation = false)
			{
				T result;
				return decode<Codec>(data, result, skipValidation);
			}


			template <class Codec, class T> static T &decode(const std::string &data, T &item, bool skipValidation = false)
			{
				static_assert(std::is_base_of<entity, T>::value,	"Item must be derived from entity");
				static_assert(std::is_base_of<codec, Codec>::value,	"Invalid codec specified");

				Codec c;

				if (skipValidation || c.validate(data))
				{
					vref<T>::decode(item, c, data, 0, -1);
				}

				return item;
			}


			template <class T> static tree to_tree(T &item)
			{
				static_assert(std::is_base_of<entity, T>::value, "Item must be derived from entity");

				return vref<T>::to_tree(item);
			}


			template <class T> static T from_tree(const tree &data)
			{
				T result;
				return from_tree(data, result);
			}


			template <class T> static T &from_tree(const tree &data, T &item)
			{
				static_assert(std::is_base_of<entity, T>::value, "Item must be derived from entity");

				vref<T>::from_tree(item, data);

				return item;
			}


		protected:

			// Abstract function that must be implemented by
			// any objects to be serialised/deserialised.
			virtual mapping describe() = 0;


			// Helper function for merging mappings when deriving
			// entities from other entities.
			inline mapping &&merge(ent::mapping &&a, const ent::mapping &b)
			{
				a.insert(b.begin(), b.end());

				return std::move(a);
			}
	};
}

