#pragma once

#include <map>
#include <stack>
#include <vector>
#include <memory>
#include <sstream>
#include <entity/codec.hpp>


namespace ent
{
	using std::string;
	using std::vector;
	using std::stack;
	using std::map;

	// Forward declaration of entity
	class entity;

	// Template conditionals
	template <class T, class enable=void> struct vref;
	template <typename T> using if_simple	= typename std::enable_if<std::is_arithmetic<T>::value || std::is_same<string, T>::value || std::is_same<vector<byte>, T>::value>::type;
	template <typename T> using if_enum		= typename std::enable_if<std::is_enum<T>::value>::type;
	template <typename T> using if_entity	= typename std::enable_if<std::is_base_of<entity, T>::value>::type;
	template <typename T> using if_map		= typename std::enable_if<std::is_same<map<string, typename T::mapped_type>, T>::value>::type;
	template <typename T> using if_vector	= typename std::enable_if<std::is_same<vector<typename T::value_type>, T>::value && !std::is_same<typename T::value_type, byte>::value>::type;


	struct vbase
	{
		virtual void encode(const codec &c, os &dst, const string &name, stack<int> &stack) = 0;
		virtual int decode(const codec &c, const string &data, int position, int type) = 0;
	};


	// Reference to any simple types (bool, number, string, vector<byte>)
	template <class T> struct vref<T, if_simple<T>> : vbase
	{
		vref(T &reference) : reference(&reference) {}


		virtual void encode(const codec &c, os &dst, const string &name, stack<int> &stack)
		{
			c.item(dst, name, *this->reference, stack.size());
		};


		static void encode(T &item, const codec &c, os &dst, const string &name, stack<int> &stack)
		{
			c.item(dst, name, item, stack.size());
		}


		virtual int decode(const codec &c, const string &data, int position, int type)
		{
			*this->reference = c.get(data, position, type, T()); return position;
		};


		static int decode(T &item, const codec &c, const string &data, int position, int type)
		{
			item = c.get(data, position, type, T()); return position;
		};


		T *reference;
	};


	/// Reference to enums (converted to/from int)
	template <class T> struct vref<T, if_enum<T>> : vbase
	{
		vref(T &reference) : reference(&reference) {}


		virtual void encode(const codec &c, os &dst, const string &name, stack<int> &stack)
		{
			c.item(dst, name, (int)*this->reference, stack.size());
		};


		static void encode(T &item, const codec &c, os &dst, const string &name, stack<int> &stack)
		{
			c.item(dst, name, (int)item, stack.size());
		}


		virtual int decode(const codec &c, const string &data, int position, int type)
		{
			*this->reference = (T)c.get(data, position, type, int()); return position;
		};


		static int decode(T &item, const codec &c, const string &data, int position, int type)
		{
			item = (T)c.get(data, position, type, int()); return position;
		};


		T *reference;
	};


	// Reference to derived entities
	template <class T> struct vref<T, if_entity<T>> : vbase
	{
		vref(T &reference) : reference(&reference) {}

		virtual void encode(const codec &c, os &dst, const string &name, stack<int> &stack)
		{
			encode(*this->reference, c, dst, name, stack);
		}

		static void encode(T &item, const codec &c, os &dst, const string &name, stack<int> &stack)
		{
			auto map	= item.describe();
			int i		= map.size() - 1;

			c.object_start(dst, name, stack);

			//for (auto &v : map.lookup)
			for (auto &v : map)
			{
				v.second->encode(c, dst, v.first, stack);
				c.separator(dst, !i--);
			}

			c.object_end(dst, stack);
		}


		virtual int decode(const codec &c, const string &data, int position, int type)
		{
			return decode(*this->reference, c, data, position, type);
		}


		static int decode(T &item, const codec &c, const string &data, int position, int type)
		{
			auto map 			= item.describe();
			std::string name	= "";

			if (c.object_start(data, position, type))
			{
				while (c.item(data, position, name, type))
				{
					if (map.count(name))
					{
						position = map[name]->decode(c, data, position, type);
					}
					else c.skip(data, position, type);
				}

				c.object_end(data, position);
			}
			else c.skip(data, position, type);

			return position;
		}


		T *reference;
	};


	// Reference to std::map
	template <class T> struct vref<T, if_map<T>> : vbase
	{
		vref(T &reference) : reference(&reference) {}


		virtual void encode(const codec &c, os &dst, const string &name, stack<int> &stack)
		{
			encode(*this->reference, c, dst, name, stack);
		}


		static void encode(T &item, const codec &c, os &dst, const string &name, stack<int> &stack)
		{
			int j = item.size() - 1;

			c.object_start(dst, name, stack);

			for (auto &i : item)
			{
				vref<typename T::mapped_type>::encode(i.second, c, dst, i.first, stack);
				c.separator(dst, !j--);
			}

			c.object_end(dst, stack);
		}


		virtual int decode(const codec &c, const string &data, int position, int type)
		{
			return decode(*this->reference, c, data, position, type);
		}


		static int decode(T &item, const codec &c, const string &data, int position, int type)
		{
			typename T::mapped_type child;
			string name = "";
			item.clear();

			if (c.object_start(data, position, type))
			{
				while (c.item(data, position, name, type))
				{
					position = vref<typename T::mapped_type>::decode(child, c, data, position, type);
					item.emplace(name, child);
				}

				c.object_end(data, position);
			}
			else c.skip(data, position, type);

			return position;
		}


		T *reference;
	};


	// Reference to std::vector (except vector<byte>)
	template <class T> struct vref<T, if_vector<T>> : vbase
	{
		vref(T &reference) : reference(&reference) {}


		virtual void encode(const codec &c, os &dst, const string &name, stack<int> &stack)
		{
			encode(*this->reference, c, dst, name, stack);
		}


		static void encode(T &item, const codec &c, os &dst, const string &name, stack<int> &stack)
		{
			int j = item.size() - 1;
			int k = 0;

			c.array_start(dst, name, stack);

			for (auto &i : item)
			{
				vref<typename T::value_type>::encode(i, c, dst, c.array_item_name(k++), stack);
				c.separator(dst, !j--);
			}

			c.array_end(dst, stack);
		}


		virtual int decode(const codec &c, const string &data, int position, int type)
		{
			return decode(*this->reference, c, data, position, type);
		}


		static int decode(T &item, const codec &c, const string &data, int position, int type)
		{
			typename T::value_type child;
			item.clear();

			if (c.array_start(data, position, type))
			{
				while (c.array_item(data, position, type))
				{
					position = vref<typename T::value_type>::decode(child, c, data, position, type);
					item.push_back(child);
				}

				c.array_end(data, position);
			}
			else c.skip(data, position, type);

			return position;
		}


		T *reference;
	};


}

