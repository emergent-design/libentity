#pragma once

#include <map>
#include <memory>
#include <iostream>
#include <algorithm>
#include <initializer_list>

#include <entity/utilities.h>


namespace ent
{
	class tree;

	class value
	{
		public:
			// The fundamental types that can be stored in the tree structure. These
			// mirror those that are supported by JSON with an additional special
			// case for binary data.
			enum class Type : byte
			{
				String, Number, Boolean, Array, Object, Null, Binary
			};


			// Indicates whether the underlying number type is integer or floating-point
			enum class Number : byte
			{
				Integer, Floating
			};


			// Constructors, default is Type::Null, the rest depend on the type of value they are initialised with
			value() {}
			value(const value &v)					: type(v.type), number(v.number),	content(v.content ? v.content->clone() : nullptr) {}
			value(const bool v) 					: type(Type::Boolean),				content(new container<bool>(v))	{}
			value(const char *v)					: type(Type::String),				content(new container<std::string>(std::string(v))) {}
			value(const std::string &v)				: type(Type::String),				content(new container<std::string>(v)) {}
			value(const std::vector<value> &v)		: type(Type::Array),				content(new container<std::vector<value>>(v)) {}
			value(const std::shared_ptr<tree> &v)	: type(Type::Object),				content(new container<std::shared_ptr<tree>>(v)) {}
			value(const std::vector<byte> &v)		: type(Type::Binary),				content(new container<std::vector<byte>>(v)) {}

			template <class T, class = typename std::enable_if<std::is_arithmetic<T>::value>::type>
				value(const T &v) :
					type(Type::Number),
					number(std::is_integral<T>::value ? Number::Integer : Number::Floating),
					content(new container<typename std::conditional<std::is_integral<T>::value, long, double>::type>(v)) {}


			// Assignment override
			value &operator=(const value &v);

			// Comparison override
			bool operator==(const value &v) const
			{
				return v.type == this->type
					&& v.number == this->number
					&& (this->type == Type::Null || this->content->compare(v.content));
			}

			~value() { delete this->content; }


			bool null() const;
			Type get_type() const;
			Number get_numtype() const;


			// Standard getters that return the stored value if the type matches the
			// requested type, otherwise the default value is returned instead.
			std::string get(const std::string &defaultValue) const;
			bool get(const bool &defaultValue) const;
			tree get(const tree &defaultValue) const;
			std::vector<byte> get(const std::vector<byte> &defaultValue) const;

			std::vector<value> &array() const;
			tree &object() const;

			// Numbers are stored as integer or double, so if an integer is requested
			// but the underlying type is a double it will be rounded.
			template <class T> typename std::enable_if<std::is_integral<T>::value, T>::type get(const T &defaultValue) const
			{
				return this->type == Type::Number
					? this->number == Number::Integer
						? static_cast<container<long> *>(this->content)->data
						: lrint(static_cast<container<double> *>(this->content)->data)
					: defaultValue;
			}

			template <class T> typename std::enable_if<std::is_floating_point<T>::value, T>::type get(const T &defaultValue) const
			{
				return this->type == Type::Number
					? this->number == Number::Integer
						? static_cast<container<long> *>(this->content)->data
						: static_cast<container<double> *>(this->content)->data
					: defaultValue;
			}


			// Confirms whether or not this value is of the given type.
			template <class T> bool is() const
			{
				if (std::is_arithmetic<T>::value)
				{
					return std::is_same<T, bool>::value ? this->type == Type::Boolean : this->type == Type::Number;
				}
				else if (std::is_same<T, std::string>::value)	return this->type == Type::String;
				else if (std::is_same<T, tree>::value)			return this->type == Type::Object;

				return false;
			}


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


			// Type information is not public since modification will
			// cause bad things when trying to get values from content.
			Type type		= Type::Null;
			Number number	= Number::Integer;
			base *content	= nullptr;			
	};
}
