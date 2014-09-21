#pragma once

#include <entity/vmap2.h>


namespace ent
{
	// Utility macros for invoking the appropriate reference constructor
	// If one argument is passed then string name of that argument is used as the mapping key
	// If two arguments are passed then the supplied name is used as the mapping key
	#ifndef ent_ref2
		#define ent_get_ref2(_1, _2, name, ...) name
		#define ent_auto_ref2(item) ent::mapping2::reference(#item, item)
		#define ent_man_ref2(name, item) ent::mapping2::reference(name, item)
		#define ent_ref2(...) ent_get_ref2(__VA_ARGS__, ent_man_ref2, ent_auto_ref2)(__VA_ARGS__)

		// Concise call to ent_ref, disable if it conflicts and use ent_ref instead
		#define eref2 ent_ref2
	#endif


	// Storage for the mapping lookup table
	class mapping2
	{
		template <class T, class U> friend struct vref;

		public:

			// Reference to a mapped variable
			struct reference
			{
				std::string name;
				std::shared_ptr<vbase> item;

				template <class T> reference(std::string name, T &item) : name(name), item(std::make_shared<vref<T>>(item)) {}
			};


			// Default
			mapping2() {}

			// Add a reference to the mapping
			mapping2 &operator<<(const reference &item)
			{
				this->lookup.emplace(item.name, item.item);
				// this->lookup[item.name] = std::move(item.item);
				return *this;
			}

		private:

			// This holds the map that links object property names to
			// actual instance members.
			std::map<std::string, std::shared_ptr<vbase>> lookup;
	};
}
