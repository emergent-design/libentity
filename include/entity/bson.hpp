#pragma once

#include <limits>
#include <entity/codec.hpp>


namespace ent
{
	struct bson : codec
	{
		using codec::item;
		using codec::object;

		static_assert(sizeof(int) == 4 && sizeof(long long) == 8 && sizeof(double) == 8, "Sizes of fundamental types are incompatible");
		static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__, "Not supported on big-endian systems");
		static const std::ios_base::openmode oflags = std::ios::out | std::ios::binary;
		const int blank = 0;

		enum Type : uint8_t
		{
			End			= 0x00,		Double		= 0x01,		String	= 0x02,		Object		= 0x03, 	Array	= 0x04,	// Supported
			Binary		= 0x05,		Boolean		= 0x08,		Null	= 0x0a,		Int32		= 0x10,		Int64	= 0x12,
			ObjectId	= 0x07,		UTC			= 0x09,		RegEx	= 0x0b,		Javascript	= 0x0d,		JsScope	= 0x0f,	// Unsupported
			Timestamp	= 0x11,		MinKey		= 0xff,		MaxKey	= 0x7f
		};


		virtual void object_start(os &dst, const string &name, stack<int> &stack) const
		{
			// The top-level object has no name
			if (!name.empty())
			{
				dst.put(Object).write(name.data(), name.size()).put(0x00);
			}

			// The stack is used to store the starting position of this object
			stack.push(dst.tellp());

			// Reserve space at the beginning for the object length
			dst.write((char *)&blank, 4);
		}


		virtual void object_end(os &dst, stack<int> &stack) const
		{
			dst.put(End);							// Write footer
			int start	= stack.top();				// Find the object start
			int length	= (int)dst.tellp() - start;	// Determine the object length
			dst.seekp(start, std::ios_base::beg);	// Jump to the start
			write(dst, length);						// Write the length
			dst.seekp(0, std::ios_base::end);		// Jump back to the end
			stack.pop();
		}

		virtual void array_start(os &dst, const string &name, stack<int> &stack) const
		{
			// The top-level array has no name
			if (!name.empty())
			{
				dst.put(Array).write(name.data(), name.size()).put(0x00);
			}

			// The stack is used to store the starting position of this array
			stack.push(dst.tellp());

			// Reserve space at the beginning for the array length
			dst.write((char *)&blank, 4);
		}

		virtual void array_end(os &dst, stack<int> &stack) const
		{
			dst.put(End);							// Write footer
			int start = stack.top();				// Find the array start
			int length = (int)dst.tellp() - start;	// Determine the array length
			dst.seekp(start, std::ios_base::beg);	// Jump to the start
			write(dst, length);						// Write the length
			dst.seekp(0, std::ios_base::end);		// Jump back to the end
			stack.pop();
		}


		virtual string array_item_name(int index) const { return std::to_string(index); }

		virtual void item(os &dst, const string &name, int) const
		{
			dst.put(Null).write(name.data(), name.size()).put(0x00);
		}

		virtual void item(os &dst, const string &name, bool value, int) const
		{
			dst.put(Boolean).write(name.data(), name.size()).put(0x00).put(value);
		}

		virtual void item(os &dst, const string &name, int32_t value, int) const
		{
			dst.put(Int32).write(name.data(), name.size()).put(0x00); write(dst, value);
		}

		virtual void item(os &dst, const string &name, int64_t value, int) const
		{
			if (value >= std::numeric_limits<int32_t>::min() && value <= std::numeric_limits<int32_t>::max())
			{
				dst.put(Int32).write(name.data(), name.size()).put(0x00); write(dst, (int32_t)value);
			}
			else
			{
				dst.put(Int64).write(name.data(), name.size()).put(0x00); write(dst, value);
			}
		}

		virtual void item(os &dst, const string &name, double value, int) const
		{
			dst.put(Double).write(name.data(), name.size()).put(0x00); write(dst, value);
		}

		virtual void item(os &dst, const string &name, const string &value, int) const
		{
			dst.put(String).write(name.data(), name.size()).put(0x00);
			write(dst, (int)value.size() + 1);
			dst.write(value.data(), value.size()).put(0x00);
		}

		virtual void item(os &dst, const string &name, const std::vector<uint8_t> &value, int) const
		{
			dst.put(Binary).write(name.data(), name.size()).put(0x00);
			write(dst, (int)value.size());
			dst.put(0x00).write((char *)value.data(), value.size());
		}


		void write(os &dst, int32_t value) const	{ dst.write((char *)&value, 4); }
		void write(os &dst, int64_t value) const	{ dst.write((char *)&value, 8); }
		void write(os &dst, double value) const		{ dst.write((char *)&value, 8); }


		virtual bool validate(const string &) const
		{
			return true;
		}


		inline uint8_t *increment(const string &s, int &i, int amount) const
		{
			uint8_t *result = (uint8_t *)s.data() + i;
			i += amount;
			return result;
		}

		inline uint8_t next(const string &s, int &i) const		{ return i < (int)s.size() ? s[i++] : error("could not read byte", i); }
		inline int32_t int32(const string &s, int &i) const		{ return i < (int)s.size() - 3 ? *(int32_t *)increment(s, i, 4) : error("could not read 32-bit integer", i); }
		inline int64_t int64(const string &s, int &i) const		{ return i < (int)s.size() - 7 ? *(int64_t *)increment(s, i, 8) : error("could not read 64-bit integer", i); }
		inline double floating(const string &s, int &i) const	{ return i < (int)s.size() - 7 ? *(double *)increment(s, i, 8)  : error("could not read floating-point value", i); }

		inline string cstring(const string &s, int &i) const
		{
			int size	= s.size();
			int j		= i;
			char *start	= (char *)s.data() + i;

			for (char *p = start; i < size && *p; p++, i++);

			return i < size ? string(start, i++ - j) : std::to_string(error("could not read cstring", j));
		}

		inline string sstring(const string &s, int &i) const
		{
			int length = int32(s, i);

			return i + length <= (int)s.size()
				? string((char *)increment(s, i, length), length-1)
				: std::to_string(error("could not read string", i));
		}

		inline vector<uint8_t> binary(const string &s, int &i) const
		{
			int length 		= int32(s, i);
			uint8_t *end 	= (uint8_t *)s.data() + i + 1 + length;

			if (i + length >= (int)s.size() || next(s, i) > 0) error("could not read binary data", i);

			return vector<uint8_t>(increment(s, i, length), end);
		}


		virtual bool object_start(const string &data, int &i, int type) const
		{
			if (type < 0 || type == Object)
			{
				return i + int32(data, i) <= (int)data.size() || error("invalid object document length", i);
			}
			return false;
		}


		virtual bool object_end(const string &, int &) const	{ return true; }
		virtual bool array_end(const string &, int &) const		{ return true; }

		virtual bool item(const string &data, int &i, string &name, int &type) const
		{
			type = next(data, i);

			if (type)
			{
				name = cstring(data, i);
				return true;
			}

			return false;
		}


		virtual bool array_start(const string &data, int &i, int type) const
		{
			if (type == Array)
			{
				return i + int32(data, i) <= (int)data.size() || error("invalid array document length", i);
			}
			return false;
		}


		virtual bool array_item(const string &data, int &i, int &type) const
		{
			type = next(data, i);

			if (type)
			{
				auto name = cstring(data, i);
				return true;
			}
			return false;
		}


		virtual int skip(const string &data, int &i, int type) const
		{
			switch (type)
			{
				case String:	increment(data, i, int32(data, i) - 4);	break;
				case Object:	increment(data, i, int32(data, i) - 4);	break;
				case Array:		increment(data, i, int32(data, i) - 4);	break;
				case Binary:	increment(data, i, int32(data, i) - 4);	break;
				case Boolean:	increment(data, i, 1);					break;
				case Int32:		increment(data, i, 4);					break;
				case Int64:		increment(data, i, 8);					break;
				case Double:	increment(data, i, 8);					break;
				case Null:												break;

				// Unsupported types
				case UTC:			increment(data, i, 8);					break;
				case Timestamp:		increment(data, i, 8);					break;
				case ObjectId:		increment(data, i, 12);					break;
				case RegEx:			cstring(data, i); cstring(data, i);		break;
				case JsScope:		increment(data, i, int32(data, i) - 4);	break;
				case Javascript:	increment(data, i, int32(data, i) - 4);	break;
				default:			break;
			}

			return 0;
		}

		virtual tree item(const string &data, int &i, int type) const
		{
			switch (type)
			{
				case String:	return get(data, i, type, string());
				case Object:	return this->object(data, i, type);
				case Array:		return this->array(data, i, type);
				case Binary:	return this->get(data, i, type, vector<uint8_t>());
				case Boolean:	return this->get(data, i, type, false);
				case Int32:		return this->get(data, i, type, int32_t());
				case Int64:		return this->get(data, i, type, int64_t());
				case Double:	return this->get(data, i, type, double());
				case Null:		return nullptr;

				// Unsupported types
				case UTC:			increment(data, i, 8);					break;
				case Timestamp:		increment(data, i, 8);					break;
				case ObjectId:		increment(data, i, 12);					break;
				case RegEx:			cstring(data, i); cstring(data, i);		break;
				case JsScope:		increment(data, i, int32(data, i) - 4);	break;
				case Javascript:	increment(data, i, int32(data, i) - 4);	break;
				default:			break;
			}

			return {};
		}

		virtual bool get(const string &data, int &i, int type, bool) const								{ return type == Boolean	? next(data, i) > 0	: skip(data, i, type); }
		virtual int32_t get(const string &data, int &i, int type, int32_t) const						{ return type == Int32		? int32(data, i)	: skip(data, i, type); }
		virtual int64_t get(const string &data, int &i, int type, int64_t) const						{ return type == Int64		? int64(data, i)	: type == Int32 ? int32(data, i) : skip(data, i, type); }
		virtual double get(const string &data, int &i, int type, double) const							{ return type == Double		? floating(data, i)	: skip(data, i, type); }
		virtual string get(const string &data, int &i, int type, const string) const					{ return type == String 	? sstring(data, i)	: string("", skip(data, i, type)); }
		virtual vector<uint8_t> get(const string &data, int &i, int type, const vector<uint8_t>) const	{ return type == Binary		? binary(data, i)	: vector<uint8_t>(skip(data, i, type)); }
		virtual bool is_null(const string &, int, int type) const 										{ return type == Null; }


		int error(const string message, int i) const
		{
			throw std::runtime_error("Error parsing bson (" + message +") at byte " + std::to_string(i));
		}
	};
}
