#pragma once

#include <map>
#include <string>
#include <cmath>
#include <stack>
#include <vector>
#include <memory>
#include <functional>
#include <entity/utilities/base64.hpp>


namespace ent
{
	using std::string;
	using std::vector;
	// using std::stack;
	using std::map;


	class tree
	{
		public:
			// The fundamental types that can be stored in the tree structure. These
			// mirror those that are supported by JSON with an additional special
			// case for binary data.
			enum class Type : uint8_t
			{
				Null, String, Integer, Floating, Boolean, Binary, Array, Object
			};

			tree() {}
			tree(tree &&value) 					: children(value.children), type(value.type), leaf(std::move(value.leaf)) {}
			tree(const tree &value) 			: children(value.children), type(value.type), leaf(value.leaf ? value.leaf->clone() : nullptr) {}
			tree(std::nullptr_t)				: type(Type::Null) {}
			tree(const bool value)				: type(Type::Boolean),	leaf(new container<bool>(value)) {}
			tree(const char *value)				: type(Type::String),	leaf(new container<string>(string(value))) {}
			tree(const string &value)			: type(Type::String),	leaf(new container<string>(value)) {}
			tree(const std::string_view value)	: type(Type::String),	leaf(new container<string>(string(value))) {}
			tree(const vector<uint8_t> &value)	: type(Type::Binary),	leaf(new container<vector<uint8_t>>(value)) {}
			tree(const vector<tree> &value) 	: type(Type::Array),	leaf(new container<vector<tree>>(value)) {}

			tree(const map<string, string> &value)
			{
				for (auto &v : value) this->children[v.first] = v.second;
			}

			tree(std::initializer_list<std::pair<const string, tree>> value) : children(value) {}

			template <class T, class = typename std::enable_if<std::is_arithmetic<T>::value>::type> tree(const T &value) :
				type(std::is_floating_point<T>::value ? Type::Floating : Type::Integer),
				leaf(new container<typename std::conditional<std::is_floating_point<T>::value, double, int64_t>::type>(value)) {}


			tree &set(const string &name, tree &&value)
			{
				this->children[name] = std::move(value);
				return *this;
			}


			tree &set(const string &name, const tree &value)
			{
				this->children[name] = value;
				return *this;
			}


			tree &erase(const string &name)
			{
				this->children.erase(name);
				return *this;
			}


			tree &operator[](const string &name)
			{
				return this->children[name];
			}


			tree &operator[](int index)
			{
				static tree empty;
				return this->type == Type::Array ? cast<vector<tree>>().at(index) : empty;
			}


			tree &at(const string &name)
			{
				return this->children.at(name);
			}


			const tree &at(const string &name) const
			{
				return this->children.at(name);
			}


			bool contains(const string &name) const
			{
				return this->children.count(name);
			}


			tree &operator=(tree &&value)
			{
				this->type		= value.type;
				this->children	= std::move(value.children);
				this->leaf		= std::move(value.leaf);
				return *this;
			}


			// Assignment override
			tree &operator=(const tree &value)
			{
				this->type		= value.type;
				this->children	= value.children;
				this->leaf		= std::unique_ptr<base>(value.leaf ? value.leaf->clone() : nullptr);
				return *this;
			}


			// Comparison override
			bool operator==(const tree &v) const
			{
				if (v.type == this->type)
				{
					if (this->null())
					{
						return true;
					}

					if (this->leaf)
					{
						return v.leaf && this->leaf->compare(v.leaf.get());
					}

					return this->children == v.children;
				}

				return this->numeric() && v.numeric() && this->as_double() == v.as_double();
			}


			bool operator!=(const tree &v) const
			{
				return !(*this == v);
			}


			bool null() const
			{
				return this->type == Type::Null;
			}


			bool numeric() const
			{
				return this->type == Type::Integer || this->type == Type::Floating;
			}


			int64_t as_long(const int64_t def = 0) const
			{
				if (!this->leaf) return def;

				switch (this->type)
				{
					case Type::String:		try { return stol(cast<string>()); } catch (...) { return def; }
					case Type::Integer:		return cast<int64_t>();
					case Type::Floating:	return lrint(cast<double>());
					case Type::Boolean:		return cast<bool>();
					default:				return def;
				}
			}


			double as_double(const double def = 0) const
			{
				if (!this->leaf) return def;

				switch (this->type)
				{
					case Type::String:		try { return stod(cast<string>()); } catch (...) { return def; }
					case Type::Integer:		return cast<int64_t>();
					case Type::Floating:	return cast<double>();
					case Type::Boolean:		return cast<bool>();
					default:				return def;
				}
			}


			bool as_bool(const bool def = false) const
			{
				if (!this->leaf) return def;

				switch (this->type)
				{
					case Type::String:		return cast<string>() == "true";
					case Type::Integer:		return cast<int64_t>() > 0;
					case Type::Floating:	return cast<double>() > 0;
					case Type::Boolean:		return cast<bool>();
					default:				return def;
				}
			}

			operator string() const { return this->as_string(); }

			string as_string(const char *def = "") const
			{
				return as_string(string(def));
			}


			string as_string(const string &def) const
			{
				if (!this->leaf) return def;

				switch (this->type)
				{
					case Type::String:		return cast<string>();
					case Type::Integer:		return std::to_string(cast<int64_t>());
					case Type::Floating:	return std::to_string(cast<double>());
					case Type::Boolean:		return cast<bool>() ? "true" : "false";
					case Type::Binary:		return base64::encode(cast<vector<uint8_t>>());
					default:				return def;
				}
			}


			vector<uint8_t> as_binary() const
			{
				switch (this->type)
				{
					case Type::Binary:	return cast<vector<uint8_t>>();
					case Type::String:	return base64::decode(cast<string>());
					default:			return {};
				}
			}


			vector<tree> &as_array() const
			{
				static vector<tree> empty;

				return this->type == Type::Array ? cast<vector<tree>>() : empty;
			}


			void as(bool &value) const				{ value = this->as_bool(); }
			void as(string &value) const			{ value = this->as_string(); }
			void as(vector<uint8_t> &value) const	{ value = this->as_binary(); }

			template <class T, class = typename std::enable_if<std::is_arithmetic<T>::value>::type> void as(T &value) const
			{
				value = std::is_floating_point<T>::value ? this->as_double() : this->as_long();
			}


			// Recursively traverse the tree. The supplied function can perform
			// operations on a child and if it returns true then this function
			// will recurse into that child.
			tree &walk(const std::function<bool(tree&)> &recurse)
			{
				if (this->type == Type::Array)
				{
					for (auto &c : cast<vector<tree>>())
					{
						if (recurse(c))
						{
							c.walk(recurse);
						}
					}
				}
				else
				{
					for (auto &c : this->children)
					{
						if (recurse(c.second))
						{
							c.second.walk(recurse);
						}
					}
				}

				return *this;
			}


			Type get_type() const { return this->type; }

			// The map of property values
			map<string, tree> children;


		private:

			// Container to hold any type of value (similar to boost::any)
			struct base
			{
				virtual ~base() {}
				virtual base *clone() = 0;
				virtual bool compare(const base *v) = 0;
			};

			template <class T> struct container : base
			{
				container(const T &value) : data(value) {}
				base *clone() { return new container<T>(this->data); }

				bool compare(const base *v) { return static_cast<const container<T> *>(v)->data == this->data; }

				T data;
			};

			template <typename T> inline T &cast() const { return static_cast<container<T> *>(this->leaf.get())->data; }


			// Type information is not public since modification will
			// cause bad things when trying to get values from leaf.
			Type type					= Type::Object;
			std::unique_ptr<base> leaf	= nullptr;
	};

}
