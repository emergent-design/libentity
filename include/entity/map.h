#pragma once

#include <entity/value.h>
#include <entity/tree.h>


namespace ent
{
	class entity;


	struct vmapbase
	{
		virtual value to() = 0;
		virtual void from(value &value) = 0;
	};


	template <class T, class enable=void> struct vmap;


	template<class T> struct vmap<T, typename std::enable_if<std::is_base_of<entity, T>::value>::type> : public vmapbase
	{
		vmap(T &reference) : reference(&reference) {}
		vmap(std::vector<T> &array) : array(&array) {}
		vmap(std::map<std::string, T> &map) : map(&map) {}


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


		T *reference					= nullptr;
		std::vector<T> *array			= nullptr;
		std::map<std::string, T> *map	= nullptr;
	};


	template <> struct vmap<std::vector<byte>> : public vmapbase
	{
		vmap(std::vector<byte> &reference) : reference(&reference) {}

		virtual value to() { return value(encode64(*reference)); }

		virtual void from(value &value) { *reference = value.get(std::vector<byte> {}); }

		std::vector<byte> *reference;
	};


	template <class T> struct vmap<T, typename std::enable_if<std::is_arithmetic<T>::value || std::is_same<std::string, T>::value>::type> : public vmapbase
	{
		vmap(T &reference) : reference(&reference) {}
		vmap(std::vector<T> &array) : array(&array) {}
		vmap(std::map<std::string, T> &map) : map(&map) {}


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


		T *reference					= nullptr;
		std::vector<T> *array			= nullptr;
		std::map<std::string, T> *map	= nullptr;
	};
}
