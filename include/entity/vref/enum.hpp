#pragma once
#include <entity/vref/base.hpp>


namespace ent
{
	/// Reference to enums (converted to/from int)
	template <class T> struct vref<T, std::enable_if_t<std::is_enum_v<T>>> : vbase
	{
		vref(T &reference) : reference(&reference) {}


		void encode(const codec &c, os &dst, const string &name, stack<int> &stack) const override
		{
			c.item(dst, name, (int)*this->reference, stack.size());
		};


		int decode(const codec &c, const string &data, int position, int type) override
		{
			return decode(*this->reference, c, data, position, type);
		};


		static void encode(T &item, const codec &c, os &dst, const string &name, stack<int> &stack)
		{
			c.item(dst, name, (int)item, stack.size());
		}

		static int decode(T &item, const codec &c, const string &data, int position, int type)
		{
			if constexpr (is_not_const<T>)
			{
				item = (T)c.get(data, position, type, int());
			}
			else
			{
				c.skip(data, position, type);
			}
			return position;
		};


		tree to_tree() const override 						{ return (int)*this->reference; }
		void from_tree(const tree &data) override			{ return from_tree(*this->reference, data); } //*this->reference = (T)data.as_long(); }
		static tree to_tree(T &item)						{ return (int)item; }
		static void from_tree(T &item, const tree &data)
		{
			if constexpr (is_not_const<T>)
			{
				item = (T)data.as_long();
			}
		}

		void modify(std::function<void(any_ref)> modifier, const bool = true) override
		{
			if constexpr (is_not_const<T>)
			{
				modifier(*this->reference);
			}
		}
		static void modify(T &item, std::function<void(any_ref)> modifier, const bool = true)
		{
			if constexpr (is_not_const<T>)
			{
				modifier(item);
			}
		}

		T *reference;
	};
}
