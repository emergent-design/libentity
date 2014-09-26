#pragma once

#include <entity/mapping.hpp>


namespace ent
{
	// The abstract base class for serialisable objects.
	// Inherit from this, implement the map function correctly,
	// and your class will become serialisable.
	class entity2
	{
		template <class T, class U> friend struct vref;

		public:

			template <class Codec, class T> static std::string encode(T &item)
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
			}


		protected:

			// Abstract function that must be implemented by
			// any objects to be serialised/deserialised.
			virtual mapping2 map() = 0;
	};
}
