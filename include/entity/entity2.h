#pragma once

#include <entity/mapping2.h>


namespace ent
{
	// The abstract base class for serialisable objects.
	// Inherit from this, implement the map function correctly,
	// and your class will become serialisable.
	class entity2
	{
		template <class T, class U> friend struct vref;

		// public:

		// 	virtual ~entity2() {}

		protected:

			// Abstract function that must be implemented by
			// any objects to be serialised/deserialised.
			virtual mapping2 map() = 0;
	};
}
