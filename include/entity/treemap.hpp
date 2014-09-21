#pragma once

#include <entity/tree2.h>
#include <entity/codec.h>


namespace ent
{

	class treemap
	{
		public:

			static void encode(const tree2 &item, const codec &c, os &dst, const string &name, stack<int> &stack)
			{
				switch (item.get_type())
				{
					case tree2::Type::Null:		c.item(dst, name, stack.size());					break;
					case tree2::Type::String:	c.item(dst, name, item.as_string(), stack.size());	break;
					case tree2::Type::Integer:	c.item(dst, name, item.as_long(), stack.size());	break;
					case tree2::Type::Floating:	c.item(dst, name, item.as_double(), stack.size());	break;
					case tree2::Type::Boolean:	c.item(dst, name, item.as_bool(), stack.size());	break;
					case tree2::Type::Binary:	c.item(dst, name, item.as_binary(), stack.size());	break;
					case tree2::Type::Array:	encode_array(item, c, dst, name, stack);			break;
					case tree2::Type::Object:	encode_object(item, c, dst, name, stack);			break;
				}
			}


			static int decode(const tree2 &item, const codec &c, const string &data, int position, int type)
			{

			}


		private:

			static void encode_object(const tree2 &item, const codec &c, os &dst, const string &name, stack<int> &stack)
			{
				int i = item.children.size() - 1;

				c.object_start(dst, name, stack);

				for (auto &child : item.children)
				{
					encode(child.second, c, dst, child.first, stack);
					c.separator(dst, !i--);
				}

				c.object_end(dst, stack);
			}

			static void encode_array(const tree2 &item, const codec &c, os &dst, const string &name, stack<int> &stack)
			{
				auto &array = item.as_array();
				int i		= array.size() - 1;
				int j		= 0;

				c.array_start(dst, name, stack);

				for (auto &child : array)
				{
					encode(child, c, dst, c.array_item_name(j++), stack);
					c.separator(dst, !i--);
				}

				c.array_end(dst, stack);
			}
	};

}

