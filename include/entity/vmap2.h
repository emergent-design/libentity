#pragma once

#include <map>
#include <vector>
#include <memory>
//#include <iostream>
#include <sstream>
//#include <entity/parser.h>
#include <entity/codec.h>
//#include <entity/entity2.h>


namespace ent
{
	// Forward declaration of entity
	class entity2;
	template <class T, class enable=void> struct vref;


	struct vbase
	{
		virtual void encode(codec &c, std::ostringstream &dst, bool pretty, int depth) {};
	};


	template <> struct vref<bool> : public vbase
	{
		vref(bool &reference) : reference(&reference) {}

		virtual void encode(codec &c, os &dst, bool pretty, int depth) 				{ c.encode(dst, *this->reference); }
		static void encode(bool &item, codec &c, os &dst, bool pretty, int depth)	{ c.encode(dst, item); }

		bool *reference;
	};


	template <class T> struct vref<T, typename std::enable_if<std::is_same<std::string, T>::value || std::is_same<std::vector<byte>, T>::value>::type> : public vbase
	{
		vref(T &reference) : reference(&reference) {}

		virtual void encode(codec &c, os &dst, bool pretty, int depth)			{ c.encode(dst, *this->reference); }
		static void encode(T &item, codec &c, os &dst, bool pretty, int depth)	{ c.encode(dst, item); }

		T *reference;
	};


	template <class T> struct vref<T, typename std::enable_if<std::is_integral<T>::value>::type> : public vbase
	{
		vref(T &reference) : reference(&reference) {}

		virtual void encode(codec &c, os &dst, bool pretty, int depth)			{ c.encode(dst, (long)*this->reference); }
		static void encode(T &item, codec &c, os &dst, bool pretty, int depth)	{ c.encode(dst, (long)item); }

		T *reference;
	};


	template <class T> struct vref<T, typename std::enable_if<std::is_floating_point<T>::value>::type> : public vbase
	{
		vref(T &reference) : reference(&reference) {}

		virtual void encode(codec &c, os &dst, bool pretty, int depth)			{ c.encode(dst, (double)*this->reference); }
		static void encode(T &item, codec &c, os &dst, bool pretty, int depth)	{ c.encode(dst, (double)item); }

		T *reference;
	};


	template <class T> struct vref<T, typename std::enable_if<std::is_base_of<entity2, T>::value>::type> : public vbase
	{
		vref(T &reference) : reference(&reference) {}

		virtual void encode(codec &c, os &dst, bool pretty, int depth)
		{
			encode(*this->reference, c, dst, pretty, depth);
		}

		static void encode(T &item, codec &c, os &dst, bool pretty, int depth)
		{
			auto map	= item.map();
			int i		= map.lookup.size() - 1;

			c.object_start(dst, pretty, depth);

			for (auto &v : map.lookup)
			{
				c.key(dst, v.first, pretty, depth + 1);
				v.second->encode(c, dst, pretty, depth + 1);
				c.separator(dst, !i--, pretty, depth);
			}

			c.object_end(dst, pretty, depth);
		}

		T *reference;
	};


	template <class T> struct vobj : public vbase
	{
		vobj(std::map<std::string, T> &reference) : reference(&reference) {}

		virtual void encode(codec &c, os &dst, bool pretty, int depth)
		{
			int j = this->reference->size() - 1;

			c.object_start(dst, pretty, depth);

			for (auto &i : *this->reference)
			{
				c.key(dst, i.first, pretty, depth + 1);
				vref<T>::encode(i.second, c, dst, pretty, depth + 1);
				c.separator(dst, !j--, pretty, depth);
			}

			c.object_end(dst, pretty, depth);
		}

		std::map<std::string, T> *reference;
	};


	template <class T> struct varr : public vbase
	{
		varr(std::vector<T> &reference) : reference(&reference) {}

		void encode(codec &c, os &dst, bool pretty, int depth)
		{
			int j = this->reference->size() - 1;

			c.array_start(dst, pretty, depth);

			for (auto &i : *this->reference)
			{
				c.item(dst, pretty, depth + 1);
				vref<T>::encode(i, c, dst, pretty, depth + 1);
				c.separator(dst, !j--, pretty, depth);
			}

			c.array_end(dst, pretty, depth);
		}

		std::vector<T> *reference;
	};
}
