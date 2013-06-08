#pragma once

#include "entity/value.h"


namespace ent
{
	class tree
	{
		public:

			tree get(const std::string name);

			tree &set(const std::string name, const tree &item);

			tree &set(const std::string name, const std::vector<byte> &item);

			tree &set(const std::string name, const std::vector<tree> &items);

			std::vector<tree> array(const std::string name);


			template <class T> T get(const std::string name, T defaultValue = T())
			{
				return this->properties.count(name) ? this->properties[name].get(defaultValue) : defaultValue;
			}


			template <class T> tree &set(const std::string name, const T&item)
			{
				this->properties[name] = value(item);
				return *this;
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


			template <class T> tree &set(std::string name, const std::vector<T> &items)
			{
				value result(vtype::Array);
				result.array.resize(items.size());

				std::transform(items.begin(), items.end(), result.array.begin(), [](const T &v) {
					return value(v);
				});

				this->properties[name] = result;

				return *this;
			}


			template <class T> std::map<std::string, T> map(const std::string name)
			{
				std::map<std::string, T> result;

				if (this->properties.count(name))
				{
					auto &p = this->properties[name];

					if (p.type == vtype::Object)
					{
						for (auto &v : p.object->properties)
						{
							if (v.second.is<T>()) result[v.first] = v.second.get(T());
						}
					}
				}

				return result;
			}


			template <class T> tree &set(std::string name, const std::map<std::string, T> &items)
			{
				tree result;

				for (auto &i : items) result.set(i.first, i.second);

				this->properties[name] = value(std::make_shared<tree>(result));

				return *this;
			}


			template <class T> std::string to(bool pretty = false) 	{ return T::to(*this, pretty); }
			template <class T> static tree from(std::string &value)	{ return T::from(value); }

			std::map<std::string, value> properties;
	};
}
