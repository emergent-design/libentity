#pragma once

//#include <map>
#include <stack>
#include <sstream>
#include <entity/utilities.h>


namespace ent
{
	typedef std::ostringstream os;
	typedef std::istringstream is;


	struct codec
	{
		static const std::ios_base::openmode oflags = std::ios::out;

		//virtual void initialise(os &dst) const = 0;
		//virtual void finalise(os &dst) const = 0;
		// virtual void key(os &dst, const std::string &name, int depth) const = 0;	// Item key
		// //virtual void separator(os &dst, bool last, int depth) const = 0;			// Item separator

		// virtual void object_start(os &dst, int depth) const = 0;
		// virtual void object_end(os &dst, int depth) const = 0;
		// virtual void array_start(os &dst, int depth) const = 0;
		// virtual void array_end(os &dst, int depth) const = 0;
		// virtual void item(os &dst, int depth) const = 0;

		// virtual void encode(os &dst) const = 0;
		// virtual void encode(os &dst, bool value) const = 0;
		// virtual void encode(os &dst, long value) const = 0;
		// virtual void encode(os &dst, double value) const = 0;
		// virtual void encode(os &dst, const std::string &value) const = 0;
		// virtual void encode(os &dst, const std::vector<byte> &value) const = 0;


		virtual std::string array_item_name(int index) const 	{ return {}; }
		virtual void separator(os &dst, bool last) const		{}	// Item separator
		virtual void object_start(os &dst, const std::string &name, std::stack<int> &stack) const = 0;
		virtual void object_end(os &dst, std::stack<int> &stack) const = 0;
		virtual void array_start(os &dst, const std::string &name, std::stack<int> &stack) const = 0;
		virtual void array_end(os &dst, std::stack<int> &stack) const = 0;
		virtual void item(os &dst, const std::string &name, int depth) const = 0;	// Array items have 0 length name
		virtual void item(os &dst, const std::string &name, bool value, int depth) const = 0;
		virtual void item(os &dst, const std::string &name, int32_t value, int depth) const = 0;
		virtual void item(os &dst, const std::string &name, int64_t value, int depth) const = 0;
		virtual void item(os &dst, const std::string &name, double value, int depth) const = 0;
		virtual void item(os &dst, const std::string &name, const std::string &value, int depth) const = 0;
		virtual void item(os &dst, const std::string &name, const std::vector<byte> &value, int depth) const = 0;
	};
}
