#pragma once
#include <entity/vref/base.hpp>


namespace ent
{
	template <typename T> using if_simple = typename std::enable_if<
		   std::is_arithmetic_v<T>
		|| std::is_same_v<string, std::remove_const_t<T>>
		|| std::is_same_v<vector<uint8_t>, std::remove_const_t<T>>
	>::type;


	// Reference to any simple types (bool, number, string, vector<uint8_t>)
	template <class T> struct vref<T, if_simple<T>> : vbase
	{
		vref(T &reference) : reference(&reference) {}


		void encode(const codec &c, os &dst, const string &name, stack<int> &stack) const override
		{
			encode(*this->reference, c, dst, name, stack);
		};

		int decode(const codec &c, const string &data, int position, int type) override
		{
			return decode(*this->reference, c, data, position, type);
		};

		static void encode(T &item, const codec &c, os &dst, const string &name, stack<int> &stack)
		{
			c.item(dst, name, item, stack.size());
		}

		static int decode(T &item, const codec &c, const string &data, int position, int type)
		{
			if constexpr (is_not_const<T>)
			{
				item = c.get(data, position, type, T());
			}
			else
			{
				// invalid_const<T>();											// Static assertion doesn't work for some reason
				// throw std::runtime_error("attempt to assign to a const");	// But runtime error does - not sure if we should use it though?
				c.skip(data, position, type);
			}
			return position;
		};

		bool is_circular(void *) const override				{ return false; }
		static bool is_circular(T &, void *)				{ return false; }

		tree to_tree() const override 						{ return *this->reference; }
		void from_tree(const tree &data) override			{ return from_tree(*this->reference, data); } //data.as(*this->reference); }
		static tree to_tree(T &item)						{ return item; }
		static void from_tree(T &item, const tree &data)
		{
			if constexpr (is_not_const<T>)
			{
				data.as(item);
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
