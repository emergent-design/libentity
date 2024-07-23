#pragma once
#include <entity/vref/base.hpp>


namespace ent
{
	template <typename T> struct is_array : std::false_type {};
	template <typename T, std::size_t N> struct is_array<std::array<T, N>> : std::true_type {};
	template <typename T> using if_array = typename std::enable_if_t<ent::is_array<std::remove_const_t<T>>::value>;


	// Reference to std::array
	template <class T> struct vref<T, if_array<T>> : vbase
	{
		vref(T &reference) : reference(&reference) {}


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
				if (c.array_start(data, position, type))
				{
					for (int i=0; c.array_item(data, position, type); i++)
					{
						if (i < (int)item.size())
						{
							position = vref<typename T::value_type>::decode(item[i], c, data, position, type);
						}
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
				if (data.get_type() == tree::Type::Array)
				{
					int i = 0;

					for (auto &d : data.as_array())
					{
						if (i < (int)item.size())
						{
							vref<typename T::value_type>::from_tree(item[i++], d);
						}
					}
				}
			}
		}


		void modify(std::function<void(any_ref)> modifier, const bool recurse = true) override
		{
			if constexpr (is_not_const<T>)
			{
				modify(*this->reference, modifier, recurse);
			}
		}


		static void modify(T &item, std::function<void(any_ref)> modifier, const bool recurse = true)
		{
			if constexpr (is_not_const<T>)
			{
				if (recurse)
				{
					for (auto &i : item)
					{
						vref<typename T::value_type>::modify(i, modifier, recurse);
					}
				}
				else
				{
					modifier(item);
				}
			}
		}


		T *reference;
	};
}
