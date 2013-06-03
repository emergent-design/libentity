#pragma once

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <initializer_list>

#ifndef byte
	typedef unsigned char byte;
#endif


class old_entity
{
	public:

		old_entity &get(const std::string name)
		{
			return this->children[name];
		}


		template <class T> T get(const std::string name, T defaultValue = T())
		{
			return this->properties.count(name) ? from_string<T>(this->properties[name]) : defaultValue;
		}


		std::vector<old_entity> &array(const std::string name)
		{
			return this->objects[name];
		}


		template <class T> std::vector<T> array(const std::string name)
		{
			if (this->arrays.count(name))
			{
				std::vector<T> result(this->arrays[name].size());
				std::transform(this->arrays[name].begin(), this->arrays[name].end(), result.begin(), from_string<T>);

				return result;
			}
			return std::vector<T>();
		}


		old_entity &set(const std::string name, const old_entity &value)
		{
			this->children[name] = value;
			return *this;
		}


		template <class T> old_entity &set(const std::string name, const T &value)
		{
			this->properties[name] = to_string(value);
			return *this;
		}


		// Store a binary vector as an encoded string
		old_entity &set(const std::string name, const std::vector<byte> &value)
		{
			this->properties[name] = encode(value);
			return *this;
		}


		old_entity &set(const std::string name, const std::vector<old_entity> &values)
		{
			this->objects[name] = values;
			return *this;
		}


		template <class T> old_entity &set(std::string name, const std::vector<T> &values)
		{
			std::vector<std::string> result(values.size());
			std::transform(values.begin(), values.end(), result.begin(), to_string<T>);
			
			this->arrays[name] = result;
			return *this;
		}


	private:

		template <class T> static std::string to_string(const T value)		{ return std::to_string(value); }
		template <class T> static T from_string(const std::string &value)	{ throw std::runtime_error("Invalid type"); }

		static std::string encode(const std::vector<byte> &value);
		static std::vector<byte> decode(const std::string &value);

		std::map<std::string, std::string> properties;				// Properties with simple values
		std::map<std::string, old_entity> children;						// Properties that are complex values
		std::map<std::string, std::vector<std::string>> arrays;		// Properties that are simple arrays
		std::map<std::string, std::vector<old_entity>> objects;			// Properties that are complex arrays
};

template <> std::string old_entity::to_string(const char *value)				{ return value; }
template <> std::string old_entity::to_string(const std::string value)			{ return value; }
template <> std::string old_entity::to_string(const bool value)					{ return value ? "true" : "false"; }

template<> std::string old_entity::from_string(const std::string &value)		{ return value; }
template<> bool old_entity::from_string(const std::string &value)				{ return value == "true"; }
template<> int old_entity::from_string(const std::string &value)				{ return std::stoi(value); }
template<> long old_entity::from_string(const std::string &value)				{ return std::stol(value); }
template<> unsigned long old_entity::from_string(const std::string &value)		{ return std::stoul(value); }
template<> long long old_entity::from_string(const std::string &value)			{ return std::stoll(value); }
template<> unsigned long long old_entity::from_string(const std::string &value)	{ return std::stoull(value); }
template<> float old_entity::from_string(const std::string &value)				{ return std::stof(value); }
template<> double old_entity::from_string(const std::string &value)				{ return std::stod(value); }
template<> long double old_entity::from_string(const std::string &value)		{ return std::stold(value); }
template<> std::vector<byte> old_entity::from_string(const std::string &value)	{ return decode(value); }
