#pragma once

//#include <map>
#include <sstream>
#include <entity/utilities.h>


namespace ent
{
	typedef std::ostringstream os;
	typedef std::istringstream is;


	struct codec
	{
		virtual void initialise(os &dst, bool pretty) = 0;
		virtual void finalise(os &dst, bool pretty) = 0;
		virtual void key(os &dst, const std::string &name, bool pretty, int depth) = 0;	// Item key
		virtual void separator(os &dst, bool last, bool pretty, int depth) = 0;			// Item separator

		virtual void object_start(os &dst, bool pretty, int depth) = 0;
		virtual void object_end(os &dst, bool pretty, int depth) = 0;
		virtual void array_start(os &dst, bool pretty, int depth) = 0;
		virtual void array_end(os &dst, bool pretty, int depth) = 0;
		virtual void item(os &dst, bool pretty, int depth) = 0;

		virtual void encode(os &dst) = 0;
		virtual void encode(os &dst, bool value) = 0;
		virtual void encode(os &dst, long value) = 0;
		virtual void encode(os &dst, double value) = 0;
		virtual void encode(os &dst, const std::string &value) = 0;
		virtual void encode(os &dst, const std::vector<byte> &value) = 0;
	};
}
