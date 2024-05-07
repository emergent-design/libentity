#pragma once


#if __has_include(<filesystem>)
#include <entity/vref/base.hpp>
#include <filesystem>

namespace ent
{
	template <typename T> using if_path = typename std::enable_if_t<std::is_same_v<std::filesystem::path, std::remove_const_t<T>>>;

	template <class T> struct vref<T, if_path<T>> : vbase
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
			c.item(dst, name, item.string(), stack.size());
		}

		static int decode(T &item, const codec &c, const string &data, int position, int type)
		{
			if constexpr (is_not_const<T>)
			{
				item = c.get(data, position, type, std::string {});
			}
			else
			{
				c.skip(data, position, type);
			}
			return position;
		};

		tree to_tree() const override 						{ return this->reference->string(); }
		void from_tree(const tree &data) override			{ return from_tree(*this->reference, data); } //data.as(*this->reference); }
		static tree to_tree(T &item)						{ return item; }
		static void from_tree(T &item, const tree &data)
		{
			if constexpr (is_not_const<T>)
			{
				item = data.as_string();
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
#endif
