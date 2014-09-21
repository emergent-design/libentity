#pragma once

#include <map>
#include <cmath>
#include <vector>
#include <memory>
#include <entity/utilities.h>

#include <iostream>

namespace ent
{
	using std::string;
	using std::vector;
	using std::map;

	class tree2
	{
		public:
			// The fundamental types that can be stored in the tree structure. These
			// mirror those that are supported by JSON with an additional special
			// case for binary data.
			enum class Type : byte
			{
				Null, String, Integer, Floating, Boolean, Binary, Array, Object
			};


			// Indicates whether the underlying number type is integer or floating-point
			// enum class Number : byte
			// {
			// 	Integer, Floating
			// };


			tree2() {}
			tree2(tree2 &&value) 					: children(value.children), type(value.type), leaf(std::move(value.leaf)) {}
			tree2(const tree2 &value) 				: children(value.children), type(value.type), leaf(value.leaf ? value.leaf->clone() : nullptr) {}
			tree2(std::nullptr_t)					: type(Type::Null) {}
			tree2(const bool value)					: type(Type::Boolean),	leaf(new container<bool>(value)) {}
			tree2(const char *value)				: type(Type::String),	leaf(new container<string>(string(value))) {}
			tree2(const string &value)				: type(Type::String),	leaf(new container<string>(value)) {}
			tree2(const vector<byte> &value)		: type(Type::Binary),	leaf(new container<vector<byte>>(value)) {}
			tree2(const vector<tree2> &value) 		: type(Type::Array),	leaf(new container<vector<tree2>>(value)) {}

			tree2(std::initializer_list<std::pair<const string, tree2>> value) : children(value) {}

			template <class T, class = typename std::enable_if<std::is_arithmetic<T>::value>::type> tree2(const T &value) :
				type(std::is_floating_point<T>::value ? Type::Floating : Type::Integer),
				leaf(new container<typename std::conditional<std::is_floating_point<T>::value, double, long>::type>(value)) {}


			tree2 &set(const string &name, tree2 &&value)
			{
				this->children.emplace(name, std::move(value));
				return *this;
			}


			tree2 &set(const string &name, const tree2 &value)
			{
				this->children.emplace(name, value);
				return *this;
			}


			tree2 &operator[](const string &name)
			{
				return this->children[name];
			}


			tree2 &operator=(tree2 &&value)
			{
				this->type		= value.type;
				this->children	= std::move(value.children);
				this->leaf		= std::move(value.leaf);
				return *this;
			}


			// Assignment override
			tree2 &operator=(const tree2 &value)
			{
				this->type		= value.type;
				this->children	= value.children;
				this->leaf		= std::unique_ptr<base>(value.leaf ? value.leaf->clone() : nullptr);
				return *this;
			}


			// Comparison override
			bool operator==(const tree2 &v) const
			{
				return v.type == this->type && v.leaf && this->leaf
					&& (this->type == Type::Null || this->leaf->compare(v.leaf.get()));
			}


			bool null() const
			{
				return this->type == Type::Null;
			}


			long as_long(const long def = 0) const
			{
				if (!this->leaf) return def;

				switch (this->type)
				{
					case Type::String:		try { return stol(cast<string>()); } catch (...) { return def; }
					case Type::Integer:		return cast<long>();
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
					case Type::Integer:		return cast<long>();
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
					case Type::Integer:		return cast<long>() > 0;
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
					case Type::Integer:		return std::to_string(cast<long>());
					case Type::Floating:	return std::to_string(cast<double>());
					case Type::Boolean:		return cast<bool>() ? "true" : "false";
					case Type::Binary:		return encode64(cast<vector<byte>>());
					default:				return def;
				}
			}


			vector<byte> &as_binary() const
			{
				static vector<byte> empty;
				empty.clear();

				return this->type == Type::Binary ? cast<vector<byte>>() : empty;
			}


			vector<tree2> &as_array() const
			{
				static vector<tree2> empty;
				empty.clear();

				return this->type == Type::Array ? cast<vector<tree2>>() : empty;
			}


			Type get_type() const { return this->type; }

			// The map of property values
			map<string, tree2> children;


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
