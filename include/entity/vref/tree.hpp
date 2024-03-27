#pragma once
#include <entity/vref/base.hpp>


namespace ent
{
	template <typename T> using if_tree	= typename std::enable_if_t<std::is_same_v<tree, std::remove_const_t<T>>>;


	template <class T> struct vref<T, if_tree<T>> : vbase
	{
		vref(T &reference) : reference(&reference) {}


		void encode(const codec &c, os &dst, const string &name, stack<int> &stack) const override
		{
			encode(*this->reference, c, dst, name, stack);
		}


		static void encode(T &item, const codec &c, os &dst, const string &name, stack<int> &stack)
		{
			c.item(item, dst, name, stack);
		}


		int decode(const codec &c, const string &data, int position, int type) override
		{
			return decode(*this->reference, c, data, position, type);
		}


		static int decode(T &item, const codec &c, const string &data, int position, int type)
		{
			if constexpr (is_not_const<T>)
			{
				item = c.item(data, position, type);
			}
			else
			{
				c.skip(data, position, type);
			}

			return position;
		}


		tree to_tree() const override						{ return *this->reference; }
		void from_tree(const tree &data) override			{ from_tree(*this->reference, data); }
		static tree to_tree(T &item)						{ return item; }
		static void from_tree(T &item, const tree &data)
		{
			if constexpr (is_not_const<T>)
			{
				item = data;
				// data.as(item);
			}
		}


		// Traversing the tree is the responsibility of the modifier function and so
		// the recurse option is ignored.
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


// template <class T> struct vref<T, if_tree<T>> : vbase
// 	{
// 		vref(T &reference) : reference(&reference) {}
// 		// vref(const T &reference) : reference(&reference) {}

// 		void encode(const codec &c, os &dst, const string &name, stack<int> &stack) const override
// 		{
// 			encode(*this->reference, c, dst, name, stack);
// 		}

// 		static void encode(T &item, const codec &c, os &dst, const string &name, stack<int> &stack)
// 		{
// 			c.item(item, dst, name, stack);
// 		}

// 		int decode(const codec &c, const string &data, int position, int type) override
// 		{
// 			*this->reference = c.item(data, position, type); return position;
// 		}

// 		static int decode(T &item, const codec &c, const string &data, int position, int type)
// 		{
// 			item = c.item(data, position, type); return position;
// 		}


// 		tree to_tree() const override						{ return *this->reference; }
// 		void from_tree(const tree &data) override			{ *this->reference = data; }
// 		static tree to_tree(T &item)						{ return item; }
// 		static void from_tree(T &item, const tree &data)	{ return data.as(item); }


// 		// Traversing the tree is the responsibility of the modifier function and so
// 		// the recurse option is ignored.
// 		void modify(std::function<void(any_ref)> modifier, const bool = true) override
// 		{
// 			modifier(*this->reference);
// 		}

// 		static void modify(T &item, std::function<void(any_ref)> modifier, const bool = true)
// 		{
// 			modifier(item);
// 		}

// 		T *reference;
// 	};
