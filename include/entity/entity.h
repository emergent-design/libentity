#pragma once

#include <entity/mapping.h>


namespace ent
{
	// The abstract base class for serialisable objects.
	// Inherit from this, implement the map function correctly,
	// and your class will become serialisable.
	class entity
	{
		public:

			virtual ~entity() {}

			// Uses the mapping to convert this object instance
			// into a tree structure for easier parsing.
			tree to()
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

			
	protected:

			// Abstract function that must be implemented by
			// any objects to be serialised/deserialised.
			virtual mapping map() = 0;
	};
}
