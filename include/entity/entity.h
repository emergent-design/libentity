#pragma once

#include "entity/value.h"


namespace ent
{
	class entity
	{
		public:

			entity get(const std::string name)
			{
				if (properties.count(name))
				{
					auto &p = this->properties[name];

					return p.type == vtype::Object ? *p.object : entity();
				}
				
				return entity();
			}


			template <class T> T get(const std::string name, T defaultValue = T())
			{
				return this->properties.count(name) ? this->properties[name].get(defaultValue) : defaultValue;
			}


			std::vector<entity> array(const std::string name)
			{
				std::vector<entity> result;

				if (this->properties.count(name))
				{
					auto &p = this->properties[name];

					if (p.type == vtype::Array)
					{
						for (auto &v : p.array)
						{
							if (v.type == vtype::Object) result.push_back(*v.object);
						}
					}
				}

				return result;
			}


			template <class T> std::vector<T> array(const std::string name)
			{
				std::vector<T> result;

				if (this->properties.count(name))
				{
					auto &p = this->properties[name];

					if (p.type == vtype::Array)
					{
						for (auto &v : p.array)
						{
							if (v.is<T>()) result.push_back(v.get(T()));
						}
					}
				}

				return result;
			}


			entity &set(const std::string name, const entity &item)
			{
				this->properties[name] = value(std::make_shared<entity>(item));
				return *this;
			}


			template <class T> entity &set(const std::string name, const T &item)
			{
				this->properties[name] = value(item);
				return *this;
			}


			// Store a binary vector as an encoded string
			entity &set(const std::string name, const std::vector<byte> &item)
			{
				this->properties[name] = value(encode64(item));
				return *this;
			}


			entity &set(const std::string name, const std::vector<entity> &items)
			{
				value result(vtype::Array);
				result.array.resize(items.size());

				std::transform(items.begin(), items.end(), result.array.begin(), [](const entity &v) {
					return value(std::make_shared<entity>(v));
				});

				this->properties[name] = result;

				return *this;
			}


			template <class T> entity &set(std::string name, const std::vector<T> &items)
			{
				value result(vtype::Array);
				result.array.resize(items.size());

				std::transform(items.begin(), items.end(), result.array.begin(), [](const T &v) {
					return value(v);
				});

				this->properties[name] = result;

				return *this;
			}


			template <class T> std::string to(bool pretty = false) 		{ return T::to(*this, pretty); }
			template <class T> static entity from(std::string &value)	{ return T::from(value); }

			std::map<std::string, value> properties;
	};
}
