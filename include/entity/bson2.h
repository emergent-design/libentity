#pragma once

#include <entity/parser.h>



namespace ent
{
	using std::string;

	struct bson2 : codec
	{
		static_assert(sizeof(int) == 4 && sizeof(long long) == 8 && sizeof(double) == 8, "Sizes of fundamental types are incompatible");
		static const std::ios_base::openmode oflags = std::ios::out | std::ios::binary;
		const int blank = 0;

		enum Type : byte
		{
			End			= 0x00,		Double		= 0x01,		String	= 0x02,		Object		= 0x03, 	Array	= 0x04,	// Supported
			Binary		= 0x05,		Boolean		= 0x08,		Null	= 0x0a,		Int32		= 0x10,		Int64	= 0x12,
			ObjectId	= 0x07,		UTC			= 0x09,		RegEx	= 0x0b,		Javascript	= 0x0d,		JsScope	= 0x0f,	// Unsupported
			Timestamp	= 0x11,		MinKey		= 0xff,		MaxKey	= 0x7f
		};


		virtual void object_start(os &dst, const string &name, std::stack<int> &stack) const
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


		virtual void object_end(os &dst, std::stack<int> &stack) const
		{
			dst.put(End);							// Write footer
			int start	= stack.top();				// Find the object start
			int length	= (int)dst.tellp() - start;	// Determine the object length
			dst.seekp(start, std::ios_base::beg);	// Jump to the start
			write(dst, length);						// Write the length
			dst.seekp(0, std::ios_base::end);		// Jump back to the end
			stack.pop();
		}

		virtual void array_start(os &dst, const string &name, std::stack<int> &stack) const
		{
			dst.put(Array).write(name.data(), name.size()).put(0x00);

			// The stack is used to store the starting position of this array
			stack.push(dst.tellp());

			// Reserve space at the beginning for the array length
			dst.write((char *)&blank, 4);
		}

		virtual void array_end(os &dst, std::stack<int> &stack) const
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

		virtual void item(os &dst, const string &name, int depth) const
		{
			dst.put(Null).write(name.data(), name.size()).put(0x00);
		}

		virtual void item(os &dst, const string &name, bool value, int depth) const
		{
			dst.put(Boolean).write(name.data(), name.size()).put(0x00).put(value);
		}

		virtual void item(os &dst, const string &name, int32_t value, int depth) const
		{
			dst.put(Int32).write(name.data(), name.size()).put(0x00); write(dst, value);
		}

		virtual void item(os &dst, const string &name, int64_t value, int depth) const
		{
			dst.put(Int64).write(name.data(), name.size()).put(0x00); write(dst, value);
		}

		virtual void item(os &dst, const string &name, double value, int depth) const
		{
			dst.put(Double).write(name.data(), name.size()).put(0x00); write(dst, value);
		}

		virtual void item(os &dst, const string &name, const string &value, int depth) const
		{
			dst.put(String).write(name.data(), name.size()).put(0x00);
			write(dst, (int)value.size() + 1);
			dst.write(value.data(), value.size()).put(0x00);
		}

		virtual void item(os &dst, const string &name, const std::vector<byte> &value, int depth) const
		{
			dst.put(Binary).write(name.data(), name.size()).put(0x00);
			write(dst, (int)value.size());
			dst.put(0x00).write((char *)value.data(), value.size());
		}

		void write(os &dst, int32_t value) const	{ value = htole32(value); dst.write((char *)&value, 4); }
		void write(os &dst, int64_t value) const	{ value = htole64(value); dst.write((char *)&value, 8); }
		void write(os &dst, double value) const		{ value = htole64(value); dst.write((char *)&value, 8); }
	};
}
