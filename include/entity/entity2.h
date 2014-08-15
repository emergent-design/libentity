#pragma once

#include <entity/mapping2.h>


namespace ent
{
	// The abstract base class for serialisable objects.
	// Inherit from this, implement the map function correctly,
	// and your class will become serialisable.
	class entity2
	{
		//friend class parser;
		//friend struct vref<entity2>;
		template <class T, class U> friend struct vref;

		public:

			virtual ~entity2() {}

			// Uses the mapping to convert this object instance
			// into a tree structure for easier parsing.
			/*tree to()
			{
				return this->map().to();
			}

			// Convenience functions to serialise this instance.
			template <class T> std::string to(bool pretty = false)
			{
				return this->map().to().to<T>(pretty);
			}

			// Uses the mapping to pull the relevant values
			// out of a tree and into this object instance.
			void from(const tree &tree)
			{
				return this->map().from(tree);
			}

			// Convenience function to deserialise this instance.
			template <class T> void from(const std::string &value)
			{
				this->map().from(T::from(value));
			}

			// Convenience function to construct an instance from serialised data
			template <class T, class U> static T from(const std::string &value)
			{
				T result;

				result.map().from(U::from(value));

				return result;
			}*/

			template <class T> std::string to(bool pretty = false)
			{
				os result;

				//vref<decltype(*this)>::encode(*this, result, pretty, 0);

				return result.str();
			}

	protected:

			// Abstract function that must be implemented by
			// any objects to be serialised/deserialised.
			virtual mapping2 map() = 0;
	};
}
