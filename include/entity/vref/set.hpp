#pragma once
#include <entity/vref/base.hpp>
#include <set>


namespace ent
{
	template <typename T> using if_set = typename std::enable_if_t<std::is_same_v<
		std::set<typename T::value_type>,
		std::remove_const_t<T>
	>>;


	template <class T> struct vref<T, if_set<T>> : vbase
	{
		vref(T &reference) : reference(&reference) {}
		// vref(const T &reference) : reference(&reference) {}


		void encode(const codec &c, os &dst, const string &name, stack<int> &stack) const override
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
				vref<const typename T::value_type>::encode(i, c, dst, c.array_item_name(k++), stack);
				c.separator(dst, !j--);
			}

			c.array_end(dst, stack);
		}


		int decode(const codec &c, const string &data, int position, int type) override
		{
			return decode(*this->reference, c, data, position, type);
		}


		static int decode(T &item, const codec &c, const string &data, int position, int type)
		{
			if constexpr (is_not_const<T>)
			{
				item.clear();

				if (c.array_start(data, position, type))
				{
					while (c.array_item(data, position, type))
					{
						typename T::value_type child;
						position = vref<typename T::value_type>::decode(child, c, data, position, type);
						item.insert(child);
					}

					c.array_end(data, position);
				}
				else
				{
					c.skip(data, position, type);
				}
			}
			else
			{
				c.skip(data, position, type);
			}

			return position;
		}


		bool is_circular(void *ancestor) const override
		{
			return is_circular(*this->reference, ancestor);
		}

		static bool is_circular(T &item, void *ancestor)
		{
			return std::any_of(item.begin(), item.end(), [=](auto &v) {
				return vref<const typename T::value_type>::is_circular(v, ancestor);
			});
		}


		tree to_tree() const override 				{ return to_tree(*this->reference); }
		void from_tree(const tree &data) override	{ from_tree(*this->reference, data); }


		static tree to_tree(T &item)
		{
			vector<tree> result;

			for (auto &i : item)
			{
				result.push_back(vref<const typename T::value_type>::to_tree(i));
			}

			return result;
		}


		static void from_tree(T &item, const tree &data)
		{
			if constexpr (is_not_const<T>)
			{
				typename T::value_type child;
				item.clear();

				if (data.get_type() == tree::Type::Array)
				{
					for (auto &i : data.as_array())
					{
						vref<typename T::value_type>::from_tree(child, i);
						item.insert(child);
					}
				}
			}
		}


		// You can't modify the items within a set so do not try recursing
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
