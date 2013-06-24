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


	// The fundamental types that can be stored in the tree structure. These
	// mirror those that are supported by JSON.
	enum class vtype
	{
		String,
		Number,
		Boolean,
		Array,
		Object,
		Null
	};


	// Storage for a single property in the tree structure.
	// Each member is named for the type of value it holds.
	struct value
	{
		vtype type;
		std::string string;
		double number;
		bool boolean;
		std::vector<value> array;
		std::shared_ptr<tree> object;

		// Constructors for each scenario, not exhaustive with the numeric types
		// but covers the commonly used ones.
		value()								: type(vtype::Null) {}
		value(vtype type)					: type(type)		{}
		value(const char *value)			: type(vtype::String),	string(value)	{}
		value(std::string value) 			: type(vtype::String),	string(value)	{}
		value(float value) 					: type(vtype::Number),	number(value)	{}
		value(double value) 				: type(vtype::Number),	number(value)	{}
		value(int value)					: type(vtype::Number),	number(value)	{}
		value(long value)					: type(vtype::Number),	number(value)	{}
		value(bool value) 					: type(vtype::Boolean),	boolean(value)	{}
		value(std::vector<value> &value)	: type(vtype::Array),	array(value)	{}
		value(std::shared_ptr<tree> value)	: type(vtype::Object),	object(value)	{}

		// Standard getters that return the stored value if the type matches the
		// requested type, otherwise the default value is returned instead.
		// Since number types are always stored as a double, when the value is
		// requested as an integer type it is rounded appropriately.
		std::vector<byte> get(std::vector<byte> defaultValue);
		std::string get(std::string defaultValue);
		float get(float defaultValue);
		double get(double defaultValue);
		int get(int defaultValue);
		long get(long defaultValue);
		bool get(bool defaultValue);
		tree get(tree defaultValue);

		// Confirms whether or not this value is of the given type.
		template <class T> bool is() { return false; }

		// Simply returns true if this is a null type
		bool null();
	};
	

	template <> bool value::is<std::string>();
	template <> bool value::is<float>();
	template <> bool value::is<double>();
	template <> bool value::is<int>();
	template <> bool value::is<long>();
	template <> bool value::is<bool>();
	template <> bool value::is<tree>();
}
