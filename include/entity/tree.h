#pragma once

#include <entity/value.h>


namespace ent
{
	// The tree is a simplistic representation of an object with property values
	// matching those found in JSON (nulls, strings, numbers, booleans, arrays
	// and objects). It can be used directly, but its main purpose is to act
	// as an intermediate stage between the object mapping and the parsers.
	class tree
	{
		public:

			// Get property by name where the value is an object and therefore
			// represented by another tree.
			tree get(const std::string name);

			// Set the value of a property to null
			tree &set(const std::string name);

			// Set the value of a property to an object
			tree &set(const std::string name, const tree &item);

			// Set the value of a property to a binary vector encoded as string
			tree &set(const std::string name, const std::vector<byte> &item);

			// Set the value of a property to an array of objects
			tree &set(const std::string name, const std::vector<tree> &items);

			// Get property by name where the value is an array of objects
			std::vector<tree> array(const std::string name);


			// Get property by name where the value is a simple scalar (string, number, boolean, null)
			template <class T> T get(const std::string name, T defaultValue = T())
			{
				return this->properties.count(name) ? this->properties[name].get(defaultValue) : defaultValue;
			}


			// Set the value of a property to a simple scalar (string, number, boolean, null)
			template <class T> tree &set(const std::string name, const T &item)
			{
				this->properties[name] = value(item);
				return *this;
			}
			

			// Get property by name where the value is an array of simple scalar values
			template <class T> std::vector<T> array(const std::string name)
			{
				std::vector<T> result;

				if (this->properties.count(name))
				{
					auto &p = this->properties[name];

					if (p.get_type() == value::Type::Array)
					{
						for (auto &v : p.array())
						{
							if (v.is<T>()) result.push_back(v.get(T()));
						}
					}
				}

				return result;
			}


			// Set the value of a property where the value is an array of simple scalar values
			template <class T> tree &set(std::string name, const std::vector<T> &items)
			{
				std::vector<value> result(items.size());
				//value result(vtype::Array);
				//result.array.resize(items.size());

				std::transform(items.begin(), items.end(), result.begin(), [](const T &v) {
					return value(v);
				});

				this->properties[name] = value(result);

				return *this;
			}


			// Get property by name where the value is an object and return as a map
			template <class T> std::map<std::string, T> map(const std::string name)
			{
				std::map<std::string, T> result;

				if (this->properties.count(name))
				{
					auto &p = this->properties[name];

					if (p.get_type() == value::Type::Object)
					{
						for (auto &v : p.object().properties)
						{
							if (v.second.is<T>()) result[v.first] = v.second.get(T());
						}
					}
				}

				return result;
			}


			// Set the value of a property via a map where the value is an object 
			template <class T> tree &set(std::string name, const std::map<std::string, T> &items)
			{
				tree result;

				for (auto &i : items) result.set(i.first, i.second);

				this->properties[name] = value(std::make_shared<tree>(result));

				return *this;
			}


			// Convenience functions to invoke the to/from of the selected parser
			template <class T> std::string to(bool pretty = false) 	{ return T::to(*this, pretty); }
			template <class T> static tree from(std::string &value)	{ return T::from(value); }


			// The map of property values
			std::map<std::string, value> properties;
	};
}

