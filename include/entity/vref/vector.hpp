#pragma once
#include <entity/vref/base.hpp>
#include <algorithm>
#include <vector>


namespace ent
{
	template <typename T> using if_vector = typename std::enable_if_t<
		    std::is_same_v<std::vector<typename T::value_type>, std::remove_const_t<T>>
		&& !std::is_same_v<typename T::value_type, uint8_t>
	>;


	// Reference to std::vector (except vector<byte>)
	template <class T> struct vref<T, if_vector<T>> : vbase
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
				// item.clear();
				const int length = item.size();

				if (c.array_start(data, position, type))
				{
					// while (c.array_item(data, position, type))
					for (int i=0; c.array_item(data, position, type); i++)
					{
						if (i < length)
						{
							position = vref<typename T::value_type>::decode(item[i], c, data, position, type);
						}
						else
						{
							typename T::value_type child;
							position = vref<typename T::value_type>::decode(child, c, data, position, type);
							item.insert(item.end(), child);
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
				const int length = item.size();
				// item.clear();

				if (data.get_type() == tree::Type::Array)
				{
					int i = 0;

					for (auto &d : data.as_array())
					{
						if (i < length)
						{
							vref<typename T::value_type>::from_tree(item[i++], d);
						}
						else
						{
							typename T::value_type child;
							vref<typename T::value_type>::from_tree(child, d);
							item.insert(item.end(), child);
						}
					}

					// for (auto &i : data.as_array())
					// {
					// 	vref<typename T::value_type>::from_tree(child, i);
					// 	item.insert(item.end(), child);
					// }
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
