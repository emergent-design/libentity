#include "entity/bson.h"
#include <endian.h>

using namespace std;


namespace ent
{
	static_assert(sizeof(int) == 4 && sizeof(long long) == 8 && sizeof(double) == 8, "Sizes of fundamental types are incompatible");


	enum Type : byte
	{
		End			= 0x00,		Double		= 0x01,		String	= 0x02,		Object		= 0x03, 	Array	= 0x04,	// Supported
		Binary		= 0x05,		Boolean		= 0x08,		Null	= 0x0a,		Int32		= 0x10,		Int64	= 0x12,
		ObjectId	= 0x07,		UTC			= 0x09,		RegEx	= 0x0b,		Javascript	= 0x0d,		JsScope	= 0x0f,	// Unsupported
		Timestamp	= 0x11,		MinKey		= 0xff,		MaxKey	= 0x7f
	};


	string bson::to(const tree &item, bool pretty, int depth)
	{
		ss result(ios::out | ios::binary);

		// Reserve space at the beginning for the document length
		write(result, 0);

		for (auto &p : item.properties) property(result, p.first, p.second);

		result.put(0x00);						// Write footer
		int length = result.tellp();			// Determine the document length
		result.seekp(0, std::ios_base::beg);	// Jump to the start
		write(result, length);					// Write the length

		return result.str();
	}


	void bson::property(ss &stream, const string &name, const value &item)
	{
		switch (item.get_type())
		{
			case value::Type::Null:		key(stream, Null, name);												break;
			case value::Type::Number:	number(stream, name, item);												break;
			case value::Type::String:	key(stream, String, name);	write(stream, item.get(string()));			break;
			case value::Type::Binary:	key(stream, Binary, name);	write(stream, item.get(vector<byte>()));	break;
			case value::Type::Boolean:	key(stream, Boolean, name);	stream.put(item.get(false));				break;
			case value::Type::Object:	key(stream, Object, name);	object(stream, item.object());				break;
			case value::Type::Array:	key(stream, Array, name);	array(stream, item.array());				break;
			default:					break;
		}
	}


	inline void bson::key(ss &stream, byte type, const string &name)	{ stream.put(type).write(name.data(), name.size()).put(0x00); }
	inline void bson::write(ss &stream, int data)						{ data = htole32(data); 						stream.write((char *)&data, 4); }
	inline void bson::write(ss &stream, long long data)					{ data = htole64(data); 						stream.write((char *)&data, 8); }
	inline void bson::write(ss &stream, double data)					{ data = htole64(data);							stream.write((char *)&data, 8); }
	inline void bson::write(ss &stream, const std::string &data)		{ int s = data.size();	write(stream, s + 1);	stream.write(data.data(), s).put(0x00); }
	inline void bson::write(ss &stream, const vector<byte> &data)		{ int s = data.size();	write(stream, s);		stream.put(0x00).write((char *)data.data(), s); }
	inline void bson::object(ss &stream, tree &item)					{ string child = to(item, false);				stream.write(child.data(), child.size()); }
	

	void bson::number(ss &stream, const string &name, const value &item)
	{
		// Figure out if the number is a floating-point, 64-bit int or small enough to fit into a 32-bit int
		byte type = item.get_numtype() == value::Number::Floating
			? Type::Double
			: abs(item.get((long long)0) <= numeric_limits<int>::max()) ? Type::Int32 : Type::Int64;

		key(stream, type, name);

		switch (type)
		{
			case Double: 	write(stream, item.get(0.0));			break;
			case Int32:		write(stream, item.get(0));				break;
			case Int64:		write(stream, item.get((long long)0));	break;
		}
	}


	void bson::array(ss &stream, const vector<value> &items)
	{
		int pos = stream.tellp();	// Store the array document position
		write(stream, 0);			// Reserve space at the beginning for the array document length

		for (int i=0; i<items.size(); i++) property(stream, to_string(i), items[i]);

		stream.put(0x00);						// Write array document footer
		int length = (int)stream.tellp() - pos;	// Find the array document length
		stream.seekp(pos, std::ios_base::beg);	// Jump to the beginning of the array document
		write(stream, length);					// Write the length
		stream.seekp(0, std::ios_base::end);	// Return to the end of the document
	}



	tree bson::from(const string &text)
	{
		blob b((byte *)text.data(), text.size());
		return parse(b);
	}


	byte *bson::blob::increment(int amount)
	{
		this->current	+= amount;
		this->remaining	-= amount;
		return this->current - amount;
	}


	byte bson::blob::next()				{ return this->remaining > 0 ? *this->increment(1)							: error("could not read byte", *this); }
	int bson::blob::int32() 			{ return this->remaining > 3 ? le32toh(*(int *)this->increment(4))			: error("could not read 32-bit integer", *this); }
	long long bson::blob::int64() 		{ return this->remaining > 7 ? le64toh(*(long long *)this->increment(8))	: error("could not read 64-bit integer", *this); }
	double bson::blob::floating() 		{ return this->remaining > 7 ? le64toh(*(double *)this->increment(8))		: error("could not read floating-point value", *this); }


	string bson::blob::cstring()
	{
		byte *pos	= this->current;
		byte *end	= pos + this->remaining;

		for (; pos < end && *pos; pos++);

		return pos < end
			? std::string((char *)this->increment(pos - this->current + 1), pos - this->current)
			: to_string(error("could not read cstring", *this));
	}


	std::string bson::blob::string()
	{
		int length = this->int32();
		return length <= this->remaining && this->current[length-1] == 0x00
			? std::string((char *)this->increment(length), length-1)
			: to_string(error("could not read string", *this));
	}

	
	std::vector<byte> bson::blob::binary()
	{
		int length	= this->int32();
		byte *end	= this->current + 1 + length;

		if (length >= this->remaining || this->next() > 0) error("could not read binary data", *this);

		return vector<byte>(this->increment(length), end);
	}


	value bson::parse_property(blob &b, byte type)
	{
		switch (type)
		{
			case Double:	return b.floating();
			case String:	return b.string();
			case Object:	return make_shared<tree>(parse(b));
			case Array:		return parse_array(b);
			case Binary:	return b.binary();
			case Boolean:	return b.next() > 0;
			case Null:		return value();
			case Int32:		return b.int32();
			case Int64:		return b.int64();

			// Unsupported types
			case UTC:			b.increment(8);				break;
			case Timestamp:		b.increment(8);				break;
			case ObjectId:		b.increment(12);			break;
			case RegEx:			b.cstring(); b.cstring(); 	break;
			case JsScope:		b.increment(b.int32() - 4);	break;
			case Javascript:	b.string();					break;
			default:			break;
		}
		
		return value();
	}


	value bson::parse_array(blob &b)
	{
		byte type;
		vector<value> result;
		int length	= b.int32();
		int count	= 0;

		if (length - 4 <= b.remaining)
		{
			byte *end = b.current + length - 4;

			while (b.current < end && (type = b.next()))
			{
				if (stoi(b.cstring()) == count++)
				{
					result.emplace_back(parse_property(b, type));
				}
				else error("invalid index in array document", b);
			}

			if (b.current < end) error("unexpected exit from array", b);
		}
		else error("invalid array document length", b);

		return value(result);
	}
	

	tree bson::parse(blob &b)
	{
		byte type;
		string name;
		tree result;
		int length = b.int32();

		if (length - 4 <= b.remaining)
		{
			byte *end = b.current + length - 4;

			while (b.current < end && (type = b.next()))
			{
				name = b.cstring();

				result.properties[name] = parse_property(b, type);
			}

			if (b.current < end) error("unexpected exit from object", b);
		}
		else error("invalid object document length", b);

		return result;
	}


	int bson::error(const string message, blob &b)
	{
		throw runtime_error("Error parsing bson (" + message +") at byte " + to_string((int)(b.current - b.data)));
	}
}

