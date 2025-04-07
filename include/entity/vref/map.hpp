#pragma once
#include <entity/vref/base.hpp>
#include <algorithm>
#include <map>


namespace ent
{
	template <typename T> using if_map = typename std::enable_if_t<std::is_same_v<
		std::map<string, typename T::mapped_type>,
		std::remove_const_t<T>
	>>;


	// Reference to std::map
	template <class T> struct vref<T, if_map<T>> : vbase
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

			c.object_start(dst, name, stack);

			for (auto &[k, v] : item)
			{
				vref<const typename T::mapped_type>::encode(v, c, dst, k, stack);
				c.separator(dst, !j--);
			}

			c.object_end(dst, stack);
		}


		int decode(const codec &c, const string &data, int position, int type) override
		{
			return decode(*this->reference, c, data, position, type);
		}


		static int decode(T &item, const codec &c, const string &data, int position, int type)
		{
			if constexpr (is_not_const<T>)
			{
				string name = "";

				if (c.object_start(data, position, type))
				{
					while (c.item(data, position, name, type))
					{
						position = vref<typename T::mapped_type>::decode(item[name], c, data, position, type);
					}

					c.object_end(data, position);
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
				return vref<const typename T::mapped_type>::is_circular(v.second, ancestor);
			});
		}


		tree to_tree() const override				{ return to_tree(*this->reference); }
		void from_tree(const tree &data) override	{ from_tree(*this->reference, data); }


		static tree to_tree(T &item)
		{
			tree result;

			for (auto &[k, v] : item)
			{
				result.set(k, vref<const typename T::mapped_type>::to_tree(v));
			}

			return result;
		}


		static void from_tree(T &item, const tree &data)
		{
			// if constexpr (is_not_const<T>)
			// {
			// 	for (auto &i : data.children)
			// 	{
			// 		vref<typename T::mapped_type>::from_tree(item[i.first], i.second);
			// 	}
			// }
			// else
			// {
			// 	static_assert(always_false<T>, "Attempt to decode to a const");
			// }

			if constexpr (std::is_const_v<T>)
			{
				// static_assert(always_false<T>, "Attempt to decode to a const");
				// invalid_const<T>();
			}
			else
			{
				for (auto &[k, v] : data.children)
				{
					vref<typename T::mapped_type>::from_tree(item[k], v);
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
						vref<typename T::mapped_type>::modify(i.second, modifier, recurse);
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
