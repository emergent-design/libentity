#pragma once

#include <entity/value.h>
#include <entity/tree.h>


// The vmap is what drives the mapping capabilities of entity. Every class/struct
// that inherits from entity must implement the create_map function in which they
// declare the mappings. Each vmap is a link between a property name and a reference
// pointer to the actual member in the object instance. This pointer is dereferenced
// whenever a value is required to populate (or be updated from) a tree.

namespace ent
{
	// Forward declaration of entity
	class entity;


	// Abstract base declaring the to/from functions so that entity can contain
	// a lookup table of name to vmap pointer (thereby allowing the concrete version
	// of to/from to be invoked).
	// The name vmap stands for value map and was used to disambiguate it from std::map.
	struct vmapbase
	{
		virtual value to() = 0;
		virtual void from(value &value) = 0;
	};


	// Required to allow 'std::enable_if<>' to do its magic.
	template <class T, class enable=void> struct vmap;


	// A value map specifically for objects which inherit from the entity base
	// class. All properties that are objects must be descended from entity
	// if they require mapping and serialising.
	template<class T> struct vmap<T, typename std::enable_if<std::is_base_of<entity, T>::value>::type> : public vmapbase
	{
		// Constructors to handle scalar, array and dictionary based values
		vmap(T &reference) : reference(&reference) {}
		vmap(std::vector<T> &array) : array(&array) {}
		vmap(std::map<std::string, T> &map) : map(&map) {}


		// Convert the actual member value in an object instance to a value
		// representation that can be stored in a tree. In this particular case
		// the member is also an object so this must be recursively converted to
		// a tree.
		virtual value to()
		{
			if (array)
			{
				value result(vtype::Array);
				result.array.resize(array->size());

				std::transform(array->begin(), array->end(), result.array.begin(), [](T &v) {
					return value(std::make_shared<tree>(v.to_tree()));
				});

				return result;
			}
			else if (map)
			{
				auto result = std::make_shared<tree>();

				for (auto &i : *map) result->set(i.first, i.second.to_tree());

				return value(result);
			}
			else if (reference) return value(std::make_shared<tree>(reference->to_tree()));

			return value();
		}


		// Update the member value in an object with data from a tree value.
		// In this particular case the member is an object and so must be
		// updated from a tree contained within the value.
		virtual void from(value &value)
		{
			if (array)
			{
				this->array->clear();

				if (value.type == vtype::Array)
				{
					for (auto &v : value.array)
					{
						if (v.type == vtype::Object)
						{
							this->array->push_back(T());
							this->array->back().from_tree(*v.object);
						}
					}
				}
			}
			else if (map)
			{
				this->map->clear();

				if (value.type == vtype::Object)
				{
					for (auto &i : value.object->properties)
					{
						if (i.second.type == vtype::Object)
						{
							(*this->map)[i.first].from_tree(*i.second.object);
						}
					}
				}
			}
			else if (value.type == vtype::Object) reference->from_tree(*value.object);
		}


		T *reference					= nullptr;	// Reference to a scalar property
		std::vector<T> *array			= nullptr;	// Reference to an array property
		std::map<std::string, T> *map	= nullptr;	// Reference to a dictionary property
	};


	// Specialised case when the type of interest is a vector<byte> since this is a
	// simple way of storing binary data. Instead of representing as a numeric array
	// it is instead converted to/from base64. It is stored as a string inside value.
	template <> struct vmap<std::vector<byte>> : public vmapbase
	{
		vmap(std::vector<byte> &reference) : reference(&reference) {}


		// Convert member value of object instance to a value representation encoded
		// as base64 in a string.
		virtual value to()
		{
			return value(encode64(*reference));
		}


		// Update the member value in the object instance by treating the string value
		// as base64.
		virtual void from(value &value)
		{
			*reference = value.get(std::vector<byte> {});
		}


		std::vector<byte> *reference;	// Reference to a binary array
	};


	// A value map for enum value types
	template <class T> struct vmap<T, typename std::enable_if<std::is_enum<T>::value>::type> : public vmapbase
	{
		// Constructors to handle scalar, array and dictionary based values
		vmap(T &reference) : reference(&reference) {}
		vmap(std::vector<T> &array) : array(&array) {}
		vmap(std::map<std::string, T> &map) : map(&map) {}

		// Convert the actual member value in an object instance to a value
		// representation that can be stored in a tree.
		virtual value to()
		{
			if (array)
			{
				value result(vtype::Array);
				result.array.resize(array->size());

				std::transform(array->begin(), array->end(), result.array.begin(), [](const T &v) {
					return value((int)v);
				});

				return result;
			}
			else if (map)
			{
				auto result = std::make_shared<tree>();

				for (auto &i : *map) result->set(i.first, (int)i.second);

				return value(result);
			}
			else if (reference)	return value((int)*reference);

			return value();
		}


		// Update the member value in an object with data from a tree value.
		virtual void from(value &value)
		{
			if (array)
			{
				this->array->clear();

				if (value.type == vtype::Array)
				{
					for (auto &v : value.array)
					{
						if (v.is<int>()) array->push_back((T)v.get(0));
					}
				}
			}
			else if (map)
			{
				this->map->clear();

				if (value.type == vtype::Object)
				{
					for (auto &i : value.object->properties)
					{
						if (i.second.is<int>()) (*this->map)[i.first] = (T)i.second.get(0);
					}
				}
			}
			else if (reference) *reference = (T)value.get(0);
		}


		T *reference					= nullptr;	// Reference to a scalar property
		std::vector<T> *array			= nullptr;	// Reference to an array property
		std::map<std::string, T> *map	= nullptr;	// Reference to a dictionary property
	};


	// A value map for any of the simple types (string, number, boolean)
	template <class T> struct vmap<T, typename std::enable_if<std::is_arithmetic<T>::value || std::is_same<std::string, T>::value>::type> : public vmapbase
	{
		// Constructors to handle scalar, array and dictionary based values
		vmap(T &reference) : reference(&reference) {}
		vmap(std::vector<T> &array) : array(&array) {}
		vmap(std::map<std::string, T> &map) : map(&map) {}


		// Convert the actual member value in an object instance to a value
		// representation that can be stored in a tree.
		virtual value to()
		{
			if (array)
			{
				value result(vtype::Array);
				result.array.resize(array->size());

				std::transform(array->begin(), array->end(), result.array.begin(), [](const T &v) {
					return value(v);
				});

				return result;
			}
			else if (map)
			{
				auto result = std::make_shared<tree>();

				for (auto &i : *map) result->set(i.first, i.second);

				return value(result);
			}
			else if (reference)	return value(*reference);

			return value();
		}


		// Update the member value in an object with data from a tree value.
		virtual void from(value &value)
		{
			if (array)
			{
				this->array->clear();

				if (value.type == vtype::Array)
				{
					for (auto &v : value.array)
					{
						if (v.is<T>()) array->push_back(v.get(T()));
					}
				}
			}
			else if (map)
			{
				this->map->clear();

				if (value.type == vtype::Object)
				{
					for (auto &i : value.object->properties)
					{
						if (i.second.is<T>()) (*this->map)[i.first] = i.second.get(T());
					}
				}
			}
			else if (reference) *reference = value.get(T());
		}


		T *reference					= nullptr;	// Reference to a scalar property
		std::vector<T> *array			= nullptr;	// Reference to an array property
		std::map<std::string, T> *map	= nullptr;	// Reference to a dictionary property
	};
}
