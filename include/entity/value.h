#pragma once

#include <map>
#include <memory>
#include <iostream>
#include <algorithm>
#include <initializer_list>

#include "entity/utilities.h"


namespace ent
{
	class entity;

	
	enum class vtype
	{
		String,
		Number,
		Boolean,
		Array,
		Object,
		Null
	};

	/*template <class T> struct vmap
	{
		std::string name;
		T *reference;
	};*/

	struct value
	{
		vtype type;
		std::string string;
		double number;
		bool boolean;
		std::vector<value> array;
		std::shared_ptr<entity> object;

		value()									: type(vtype::Null) {}
		value(vtype type)						: type(type)		{}
		value(const char *value)				: type(vtype::String),	string(value)	{}
		value(std::string value) 				: type(vtype::String),	string(value)	{}
		value(float value) 						: type(vtype::Number),	number(value)	{}
		value(double value) 					: type(vtype::Number),	number(value)	{}
		value(int value)						: type(vtype::Number),	number(value)	{}
		value(long value)						: type(vtype::Number),	number(value)	{}
		value(bool value) 						: type(vtype::Boolean),	boolean(value)	{}
		value(std::vector<value> &value)		: type(vtype::Array),	array(value)	{}
		value(std::shared_ptr<entity> value)	: type(vtype::Object),	object(value)	{}

		bool null();

		std::string get(std::string defaultValue);
		float get(float defaultValue);
		double get(double defaultValue);
		int get(int defaultValue);
		long get(long defaultValue);
		bool get(bool defaultValue);

		std::vector<byte> get(std::vector<byte> defaultValue);

		template <class T> bool is() { return false; }
	};


	template <> bool value::is<std::string>();
	template <> bool value::is<float>();
	template <> bool value::is<double>();
	template <> bool value::is<int>();
	template <> bool value::is<long>();
	template <> bool value::is<bool>();


	inline std::ostream &operator << (std::ostream &output, const value &v)
	{
		switch (v.type)
		{
			case vtype::String:		return output << '"' << v.string << '"';
			case vtype::Number:		return output << v.number;
			case vtype::Boolean:	return output << (v.boolean ? "true" : "false");
			case vtype::Null:		return output << "null";
			case vtype::Object:		return output << "object";
			case vtype::Array:		return output << "array";
			default:				return output;
		}
	}
}
