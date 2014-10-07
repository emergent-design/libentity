#pragma once

#include <stack>
#include <sstream>
#include <entity/tree.hpp>
#include <entity/utilities.hpp>


namespace ent
{
	using std::string;
	using std::vector;
	using std::stack;


	struct codec
	{
		static const std::ios_base::openmode oflags = std::ios::out;

		// Encoding functions
		virtual string array_item_name(int index) const 	{ return {}; }
		virtual void separator(os &dst, bool last) const	{}	// Item separator
		virtual void object_start(os &dst, const string &name, stack<int> &stack) const = 0;
		virtual void object_end(os &dst, stack<int> &stack) const = 0;
		virtual void array_start(os &dst, const string &name, stack<int> &stack) const = 0;
		virtual void array_end(os &dst, stack<int> &stack) const = 0;
		virtual void item(os &dst, const string &name, int depth) const = 0;	// Array items have 0 length name
		virtual void item(os &dst, const string &name, bool value, int depth) const = 0;
		virtual void item(os &dst, const string &name, int32_t value, int depth) const = 0;
		virtual void item(os &dst, const string &name, int64_t value, int depth) const = 0;
		virtual void item(os &dst, const string &name, double value, int depth) const = 0;
		virtual void item(os &dst, const string &name, const string &value, int depth) const = 0;
		virtual void item(os &dst, const string &name, const vector<byte> &value, int depth) const = 0;


		// Decoding functions
		virtual bool validate(const string &data) const = 0;
		virtual bool object_start(const string &data, int &i, int type) const = 0;
		virtual bool object_end(const string &data, int &i) const = 0;
		virtual bool item(const string &data, int &i, string &name, int &type) const = 0;
		virtual bool array_start(const string &data, int &i, int type) const = 0;
		virtual bool array_end(const string &data, int &i) const = 0;
		virtual bool array_item(const string &data, int &i, int &type) const = 0;
		virtual int skip(const string &data, int &i, int type) const = 0;

		virtual bool get(const string &data, int &i, int type, bool def) const = 0;
		virtual int32_t get(const string &data, int &i, int type, int32_t def) const = 0;
		virtual int64_t get(const string &data, int &i, int type, int64_t def) const = 0;
		virtual double get(const string &data, int &i, int type, double def) const = 0;
		virtual string get(const string &data, int &i, int type, const string def) const = 0;
		virtual vector<byte> get(const string &data, int &i, int type, const vector<byte> def) const = 0;


		// Encode dynamic type
		void object(const tree &item, os &dst, const string &name, stack<int> &stack) const
		{
			int i = item.children.size() - 1;

			this->object_start(dst, name, stack);

			for (auto &child : item.children)
			{
				this->item(child.second, dst, child.first, stack);
				this->separator(dst, !i--);
			}

			this->object_end(dst, stack);
		}


		void array(const tree &item, os &dst, const string &name, stack<int> &stack) const
		{
			auto &array = item.as_array();
			int i		= array.size() - 1;
			int j		= 0;

			this->array_start(dst, name, stack);

			for (auto &child : array)
			{
				this->item(child, dst, this->array_item_name(j++), stack);
				this->separator(dst, !i--);
			}

			this->array_end(dst, stack);
		}


		void item(const tree &item, os &dst, const string &name, stack<int> &stack) const
		{
			switch (item.get_type())
			{
				case tree::Type::Null:		this->item(dst, name, stack.size());					break;
				case tree::Type::String:	this->item(dst, name, item.as_string(), stack.size());	break;
				case tree::Type::Integer:	this->item(dst, name, item.as_long(), stack.size());	break;
				case tree::Type::Floating:	this->item(dst, name, item.as_double(), stack.size());	break;
				case tree::Type::Boolean:	this->item(dst, name, item.as_bool(), stack.size());	break;
				case tree::Type::Binary:	this->item(dst, name, item.as_binary(), stack.size());	break;
				case tree::Type::Array:		this->array(item, dst, name, stack);					break;
				case tree::Type::Object:	this->object(item, dst, name, stack);					break;
			}
		}


		// Decode dynamic type
		virtual tree item(const string &data, int &i, int type) const = 0;

		tree object(const string &data, int &i, int type) const
		{
			string name;
			tree result;

			if (this->object_start(data, i, type))
			{
				while (this->item(data, i, name, type))
				{
					result.set(name, this->item(data, i, type));
				}

				this->object_end(data, i);
			}
			else this->skip(data, i, type);

			return result;
		}


		vector<tree> array(const string &data, int &i, int type) const
		{
			string name;
			vector<tree> result;

			if (this->array_start(data, i, type))
			{
				while (this->array_item(data, i, type))
				{
					result.push_back(this->item(data, i, type));
				}

				this->array_end(data, i);
			}
			else this->skip(data, i, type);

			return result;
		}
	};
}

