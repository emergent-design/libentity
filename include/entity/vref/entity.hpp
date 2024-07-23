#pragma once
#include <entity/vref/base.hpp>


namespace ent
{
	template <typename T> using if_entity = typename std::enable_if_t<std::is_same_v<mapping, decltype(T().ent_describe())>>;


	// Reference to derived entities
	template <class T> struct vref<T, if_entity<T>> : vbase
	{
		vref(T &reference) : reference(&reference) {}
		// vref(const T &reference) : reference(&reference) {}

		void encode(const codec &c, os &dst, const string &name, stack<int> &stack) const override
		{
			encode(*this->reference, c, dst, name, stack);
		}


		static void encode(T &item, const codec &c, os &dst, const string &name, stack<int> &stack)
		{
			auto map	= item.ent_describe();
			int i		= map.size() - 1;

			c.object_start(dst, name, stack);

			//for (auto &v : map.lookup)
			for (auto &[k, v] : map)
			{
				v->encode(c, dst, k, stack);
				c.separator(dst, !i--);
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
				auto map 			= item.ent_describe();
				std::string name	= "";

				if (c.object_start(data, position, type))
				{
					while (c.item(data, position, name, type))
					{
						if (map.count(name))
						{
							position = map[name]->decode(c, data, position, type);
						}
						else
						{
							c.skip(data, position, type);
						}
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
			for (const auto &[k,v] : item.ent_describe())
			{
				if (v->is_circular(ancestor))
				{
					return true;
				}
			}

			return false;
		}


		tree to_tree() const override				{ return to_tree(*this->reference); }
		void from_tree(const tree &data) override	{ from_tree(*this->reference, data); }


		static tree to_tree(T &item)
		{
			tree result;

			for (auto &[k, v] : item.ent_describe())
			{
				result.set(k, v->to_tree());
			}

			return result;
		}


		static void from_tree(T &item, const tree &data)
		{
			if constexpr (is_not_const<T>)
			{
				for (auto &[k, v] : item.ent_describe())
				{
					if (data.contains(k))
					{
						v->from_tree(data.at(k));
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
					for (auto &i : item.ent_describe())
					{
						i.second->modify(modifier, recurse);
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
