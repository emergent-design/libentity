#pragma once

//#include <map>
#include <stack>
#include <sstream>
#include <entity/utilities.h>


namespace ent
{
	using std::string;
	using std::vector;
	using std::stack;

	typedef std::ostringstream os;
	typedef std::istringstream is;


	struct codec
	{
		static const std::ios_base::openmode oflags = std::ios::out;

		// Encoding functions
		virtual string array_item_name(int index) const 	{ return {}; }
		virtual void separator(os &dst, bool last) const		{}	// Item separator
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
	};
}

