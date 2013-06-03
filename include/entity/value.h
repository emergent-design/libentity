#pragma once

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <initializer_list>

#ifndef byte
	typedef unsigned char byte;
#endif


namespace ent
{
	class entity;

	std::string encode64(const std::vector<byte> &value);
	std::vector<byte> decode64(const std::string &value);

	
	enum class vtype
	{
		String,
		//Float,
		//Integer,
		Number,
		Boolean,
		Array,
		Object,
		Null
	};


	struct value
	{
		vtype type;
		std::string string;
		//double floating;
		//long integer;
		double number;
		bool boolean;
		std::vector<value> array;
		std::shared_ptr<entity> object;

		value()									: type(vtype::Null) {}
		value(const char *value)				: type(vtype::String),	string(value)	{}
		value(std::string value) 				: type(vtype::String),	string(value)	{}
		//value(float value) 						: type(vtype::Float),	floating(value)	{}
		//value(double value) 					: type(vtype::Float),	floating(value)	{}
		//value(int value)						: type(vtype::Integer),	integer(value)	{}
		//value(long value)						: type(vtype::Integer),	integer(value)	{}
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
}
