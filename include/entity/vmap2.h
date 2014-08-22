#pragma once

#include <map>
#include <vector>
#include <memory>
#include <sstream>
#include <entity/codec.h>

#include <iostream>


namespace ent
{
	using std::string;

	// Forward declaration of entity
	class entity2;
	template <class T, class enable=void> struct vref;


	struct vbase
	{
		virtual void encode(const codec &c, os &dst, const string &name, std::stack<int> &stack) = 0;
		virtual int decode(const codec &c, const string &data, int position, int type) { return position; }; //= 0;
	};


	template <> struct vref<bool> : public vbase
	{
		vref(bool &reference) : reference(&reference) {}

		virtual void encode(const codec &c, os &dst, const string &name, std::stack<int> &stack) 			{ c.item(dst, name, *this->reference, stack.size()); };
		static void encode(bool &item, const codec &c, os &dst, const string &name, std::stack<int> &stack)	{ c.item(dst, name, item, stack.size()); }

		virtual int decode(const codec &c, const string &data, int position, int type) 				{ *this->reference	= c.get(data, position, type, false);	return position; };
		static int decode(bool &item, const codec &c, const string &data, int position, int type) 	{ item				= c.get(data, position, type, false); 	return position; };

		bool *reference;
	};


	template <class T> struct vref<T, typename std::enable_if<std::is_same<string, T>::value || std::is_same<std::vector<byte>, T>::value>::type> : public vbase
	{
		vref(T &reference) : reference(&reference) {}

		virtual void encode(const codec &c, os &dst, const string &name, std::stack<int> &stack) 			{ c.item(dst, name, *this->reference, stack.size()); };
		static void encode(T &item, const codec &c, os &dst, const string &name, std::stack<int> &stack)	{ c.item(dst, name, item, stack.size()); }

		virtual int decode(const codec &c, const string &data, int position, int type) 			{ *this->reference	= c.get(data, position, type, T());	return position; };
		static int decode(T &item, const codec &c, const string &data, int position, int type) 	{ item				= c.get(data, position, type, T()); return position; };

		T *reference;
	};


	// template <class T> struct vref<T, typename std::enable_if<std::is_integral<T>::value>::type> : public vbase//
	template <class T> struct vref<T, typename std::enable_if<std::is_arithmetic<T>::value>::type> : public vbase
	{
		vref(T &reference) : reference(&reference) {}

		virtual void encode(const codec &c, os &dst, const string &name, std::stack<int> &stack) 			{ c.item(dst, name, *this->reference, stack.size()); };
		static void encode(T &item, const codec &c, os &dst, const string &name, std::stack<int> &stack)	{ c.item(dst, name, item, stack.size()); }

		virtual int decode(const codec &c, const string &data, int position, int type) 			{ *this->reference	= c.get(data, position, type, T()); return position; };
		static int decode(T &item, const codec &c, const string &data, int position, int type) 	{ item				= c.get(data, position, type, T()); return position; };

		T *reference;
	};


	/*template <class T> struct vref<T, typename std::enable_if<std::is_floating_point<T>::value>::type> : public vbase
	{
		vref(T &reference) : reference(&reference) {}

		virtual void encode(const codec &c, os &dst, const string &name, std::stack<int> &stack) 			{ c.item(dst, name, *this->reference, stack.size()); };
		static void encode(T &item, const codec &c, os &dst, const string &name, std::stack<int> &stack)	{ c.item(dst, name, item, stack.size()); }

		virtual int decode(const codec &c, const string &data, int position, int type) 				{ return c.get(data, position, type, *this->reference); };
		static int decode(bool &item, const codec &c, const string &data, int position, int type) 	{ return c.get(data, position, type, item); };

		T *reference;
	};*/


	template <class T> struct vref<T, typename std::enable_if<std::is_base_of<entity2, T>::value>::type> : public vbase
	{
		vref(T &reference) : reference(&reference) {}

		virtual void encode(const codec &c, os &dst, const string &name, std::stack<int> &stack)
		{
			encode(*this->reference, c, dst, name, stack);
		}

		static void encode(T &item, const codec &c, os &dst, const string &name, std::stack<int> &stack)
		{
			auto map	= item.map();
			int i		= map.lookup.size() - 1;

			c.object_start(dst, name, stack);

			for (auto &v : map.lookup)
			{
				v.second->encode(c, dst, v.first, stack);
				c.separator(dst, !i--);
			}

			c.object_end(dst, stack);
		}


		virtual void decode(const codec &c, const string &data, int &position, int type)
		{
			decode(*this->reference, c, data, position, type);
		}


		static int decode(T &item, const codec &c, const string &data, int position, int type)
		{
			auto map 			= item.map();
			std::string name	= "";

			if (c.object_start(data, position))
			{
				while (c.item(data, position, name, type))
				{
					if (map.lookup.count(name))
					{
						position = map.lookup[name]->decode(c, data, position, type);
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


	template <class T> struct vobj : public vbase
	{
		vobj(std::map<string, T> &reference) : reference(&reference) {}

		virtual void encode(const codec &c, os &dst, const string &name, std::stack<int> &stack)
		{
			int j = this->reference->size() - 1;

			c.object_start(dst, name, stack);

			for (auto &i : *this->reference)
			{
				vref<T>::encode(i.second, c, dst, i.first, stack);
				c.separator(dst, !j--);
			}

			c.object_end(dst, stack);
		}

		std::map<string, T> *reference;
	};


	template <class T> struct varr : public vbase
	{
		varr(std::vector<T> &reference) : reference(&reference) {}

		virtual void encode(const codec &c, os &dst, const string &name, std::stack<int> &stack)
		{
			int j = this->reference->size() - 1;
			int k = 0;

			c.array_start(dst, name, stack);

			for (auto &i : *this->reference)
			{
				vref<T>::encode(i, c, dst, c.array_item_name(k++), stack);
				c.separator(dst, !j--);
			}

			c.array_end(dst, stack);
		}

		std::vector<T> *reference;
	};
}
