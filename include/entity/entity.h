#pragma once

#include <entity/map.h>
#include <entity/tree.h>


namespace ent
{
	// The abstract base class for serialisable objects.
	// Inherit from this, implement the create_map function correctly,
	// and your class will become serialisable.
	class entity
	{
		public:

			// Convenience function that invokes the appropriate
			// functions to serialise this instance.
			template <class T> std::string to(bool pretty = false)
			{
				return this->to_tree().to<T>(pretty);
			}

			// Convenience function that invokes the appropriate
			// functions to deserialise this instance.
			template <class T> void from(std::string &value)
			{
				tree t = T::from(value);
				this->from_tree(t); 
			}

			// Uses the mapping to convert this object instance
			// into a tree structure for easier parsing.
			tree to_tree();

			// Uses the mapping to pull the relevant values
			// out of a tree and into this object instance.
			void from_tree(tree &tree);

			// Abstract function that must be implemented by
			// any objects to be serialised/deserialised.
			virtual void create_map() = 0;

			
		protected:

			// Create a mapping instance for a singular value
			template <class T> void map(std::string name, T &reference)
			{
				this->mapping[name] = std::make_shared<vmap<T>>(reference);
			}

			// Create a mapping instance for an array
			template <class T> void map(std::string name, std::vector<T> &reference)
			{
				this->mapping[name] = std::make_shared<vmap<T>>(reference);
			}

			// Create a mapping instance for a dictionary
			template <class T> void map(std::string name, std::map<std::string, T> &reference)
			{
				this->mapping[name] = std::make_shared<vmap<T>>(reference);
			}


		private:

			// This holds the map that links object property names to
			// actual instance members.
			std::map<std::string, std::shared_ptr<vmapbase>> mapping;
	};

	// Special case, a vector of byte is converted to base64 and stored as a string
	template <> void entity::map(std::string name, std::vector<byte> &reference);
}
