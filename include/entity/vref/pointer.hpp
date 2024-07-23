#pragma once
#include <entity/vref/base.hpp>


namespace ent
{
	template <typename T> using if_pointer = typename std::enable_if_t<
		   std::is_same_v<std::remove_const_t<T>, std::shared_ptr<typename T::element_type>>
		|| std::is_same_v<std::remove_const_t<T>, std::unique_ptr<typename T::element_type>>
	>;



	// Reference to smart pointers
	template <class T> struct vref<T, if_pointer<T>> : vbase
	{
		vref(T &reference) : reference(&reference) {}


		void encode(const codec &c, os &dst, const string &name, stack<int> &stack) const override
		{
			encode(*this->reference, c, dst, name, stack);
		}


		static void encode(T &item, const codec &c, os &dst, const string &name, stack<int> &stack)
		{
			if (item)
			{
				if (vref<const typename T::element_type>::is_circular(*item, item.get()))
				{
					throw std::runtime_error(
						"Circular reference found back to '" + name + "', aborting encode"
					);
				}

				vref<typename T::element_type>::encode(*item, c, dst, name, stack);
			}
			else
			{
				c.item(dst, name, stack.size()); // null
			}
		}


		int decode(const codec &c, const string &data, int position, int type) override
		{
			return decode(*this->reference, c, data, position, type);
		}


		static T make_pointer()
		{
			if constexpr (std::is_same_v<T, std::unique_ptr<typename T::element_type>>)
			{
				return std::make_unique<typename T::element_type>();
			}
			else
			{
				return std::make_shared<typename T::element_type>();
			}
		}


		static int decode(T &item, const codec &c, const string &data, int position, int type)
		{
			if constexpr (is_not_const<T>)
			{
				if (c.is_null(data, position, type))
				{
					item = nullptr;
					c.skip(data, position, type);
				}
				else if (item)
				{
					if (vref<const typename T::element_type>::is_circular(*item, item.get()))
					{
						throw std::runtime_error("Circular reference found aborting decode");
					}

					// If the item already exists then decode into it
					position = vref<typename T::element_type>::decode(*item, c, data, position, type);
				}
				else
				{
					// Otherwise create a new instance
					item = make_pointer();
					position = vref<typename T::element_type>::decode(*item, c, data, position, type);
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
			return item && (item.get() == ancestor || vref<typename T::element_type>::is_circular(*item, ancestor));
		}


		tree to_tree() const override
		{
			return to_tree(*this->reference);
		}

		void from_tree(const tree &data) override
		{
			from_tree(*this->reference, data);
		}


		static tree to_tree(T &item)
		{
			return item ? vref<typename T::element_type>::to_tree(*item) : tree {};
		}


		static void from_tree(T &item, const tree &data)
		{
			if constexpr (is_not_const<T>)
			{
				if (data.null())
				{
					item = nullptr;
				}
				else if (item)
				{
					if (vref<const typename T::element_type>::is_circular(*item, item.get()))
					{
						throw std::runtime_error("Circular reference found aborting from_tree");
					}


					vref<typename T::element_type>::from_tree(*item, data);
				}
				else
				{
					item = make_pointer();
					vref<typename T::element_type>::from_tree(*item, data);
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
				if (item)
				{
					vref<typename T::element_type>::modify(*item, modifier, recurse);
				}
			}
		}


		T *reference;
	};


}
