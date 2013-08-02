#pragma once

#include <entity/vmap.h>
#include <entity/tree.h>


namespace ent
{
	// Utility macros for invoking the appropriate reference constructor
	// If one argument is passed then string name of that argument is used as the mapping key
	// If two arguments are passed then the supplied name is used as the mapping key
	#ifndef ent_ref
		#define ent_get_ref(_1, _2, name, ...) name
		#define ent_auto_ref(item) ent::mapping::reference(#item, item)
		#define ent_man_ref(name, item) ent::mapping::reference(name, item)
		#define ent_ref(...) ent_get_ref(__VA_ARGS__, ent_man_ref, ent_auto_ref)(__VA_ARGS__)

		// Concise call to ent_ref, disable if it conflicts and use ent_ref instead
		#define eref ent_ref
	#endif


	// Storage for the mapping lookup table
	class mapping
	{
		public:

			// Reference to a mapped variable
			struct reference
			{
				std::string name;
				std::shared_ptr<vmapbase> item;

				// Create a mapping reference for a singular value
				template <class T> reference(std::string name, T &item) : name(name), item(std::make_shared<vmap<T>>(item)) {}

				// Create a mapping reference for an array (except vector<byte>)
				template <class T, class = typename std::enable_if<!std::is_same<T, byte>::value>::type>
					reference(std::string name, std::vector<T> &item) : name(name), item(std::make_shared<vmap<T>>(item)) {}

				// Create a mapping reference for a dictionary
				template <class T> reference(std::string name, std::map<std::string, T> &item) : name(name), item(std::make_shared<vmap<T>>(item)) {}
			};


			// Default
			mapping() {}

			// Construct from a single reference
			mapping(const reference &item);

			// Construct from a list of references
			mapping(std::initializer_list<reference> items);

			// Add a reference to the mapping
			mapping &operator<<(const reference &item);

			// Add a list of references to the mapping
			mapping &operator<<(std::initializer_list<reference> items);

			// Traverses the map to generate a tree
			tree to();

			// Traverses the map and updates with values from the tree
			void from(const tree &tree);

		private:

			// This holds the map that links object property names to
			// actual instance members.
			std::map<std::string, std::shared_ptr<vmapbase>> lookup;
	};
}
