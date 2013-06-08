#pragma once

//#include <map>
//#include <memory>
//#include <iostream>
//#include <algorithm>
//#include <initializer_list>

#include "entity/value.h"


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
		vmap(std::vector<T> &array) : array(&array) {}
		vmap(T &reference) : reference(&reference) {}


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
			return value(std::make_shared<tree>(reference->to_tree()));
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
							T item;
							item.from_tree(*v.object);
							
							this->array->push_back(item);
						}
					}
				}
			}
			else if (value.type == vtype::Object) reference->from_tree(*value.object);
		}


		T *reference			= nullptr;
		std::vector<T> *array	= nullptr;
	};


	template <> struct vmap<std::vector<byte>> : public vmapbase
	{
		vmap(std::vector<byte> &reference) : reference(&reference) {}

		virtual value to()
		{
			return value(encode64(*reference));
		}

		virtual void from(value &value)
		{
			*reference = value.get(std::vector<byte> {});
		}

		std::vector<byte> *reference;
	};


	template <class T> struct vmap<T, typename std::enable_if<std::is_arithmetic<T>::value || std::is_same<std::string, T>::value>::type> : public vmapbase
	{
		vmap(std::vector<T> &array) : array(&array) {}
		vmap(T &reference) : reference(&reference) {}

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
			return value(*reference);
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
			else *reference = value.get(T());
		}


		T *reference			= nullptr;
		std::vector<T> *array	= nullptr;
	};
}
