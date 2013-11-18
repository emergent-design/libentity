#include <xUnit++/xUnit++.h>
#include <entity/entity.h>
#include <entity/bson.h>

using namespace std;
using namespace ent;



SUITE("BSON Tests")
{
	string convert(vector<byte> &data) { return string((char *)data.data(), data.size()); }

	vector<tuple<vector<byte>, tree>> simple_vectors()
	{
		return {				  // |      Length       |Type|  Name   |
			make_tuple(vector<byte> { 0x0c,0x00,0x00,0x00,0x10,0x61,0x00,0x2a,0x00,0x00,0x00,0x00 },						tree().set("a", 42)),							// Int32
			make_tuple(vector<byte> { 0x10,0x00,0x00,0x00,0x12,0x61,0x00,0x34,0x1c,0xdc,0xdf,0x02,0x00,0x00,0x00,0x00 },	tree().set("a", 12345678900)),					// Int64
			make_tuple(vector<byte> { 0x10,0x00,0x00,0x00,0x01,0x61,0x00,0x1f,0x85,0xeb,0x51,0xb8,0x1e,0x09,0x40,0x00 },	tree().set("a", 3.14)),							// Double
			make_tuple(vector<byte> { 0x0e,0x00,0x00,0x00,0x02,0x61,0x00,0x02,0x00,0x00,0x00,0x62,0x00,0x00 }, 				tree().set("a", "b")),							// String
			make_tuple(vector<byte> { 0x09,0x00,0x00,0x00,0x08,0x61,0x00,0x01,0x00 }, 										tree().set("a", true)),							// Boolean
			make_tuple(vector<byte> { 0x08,0x00,0x00,0x00,0x0a,0x61,0x00,0x00 },											tree().set("a")),								// Null
			make_tuple(vector<byte> { 0x0f,0x00,0x00,0x00,0x05,0x61,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0xff,0x00 },			tree().set("a", vector<byte> { 0x00, 0xff })),	// Binary
		}; 
	}

								// |      Length       |Type|  Name   |      Length       |Type|  Name   |       Value       |Foot|Foot|
	vector<byte> OBJECT_TREE 	= { 0x14,0x00,0x00,0x00,0x03,0x61,0x00,0x0c,0x00,0x00,0x00,0x10,0x62,0x00,0x2a,0x00,0x00,0x00,0x00,0x00 };
	vector<byte> ARRAY_TREE		= { 0x14,0x00,0x00,0x00,0x04,0x61,0x00,0x0c,0x00,0x00,0x00,0x10,0x30,0x00,0x2a,0x00,0x00,0x00,0x00,0x00 };


	DATA_THEORY("Simple types can be serialised", (vector<byte> expected, tree value), simple_vectors)
	{
		Assert.Equal(convert(expected), bson::to(value, false));
	}


	DATA_THEORY("Simple types can be deserialised", (vector<byte> data, tree expected), simple_vectors)
	{
		Assert.Equal(expected.properties["a"], bson::from(convert(data)).properties["a"]);
	}


	FACT("Arrays can be serialised")
	{
		Assert.Equal(convert(ARRAY_TREE), bson::to(tree().set("a", vector<int> { 42 }), false));
	}


	FACT("Arrays can be deserialised")
	{
		Assert.Equal(42, bson::from(convert(ARRAY_TREE)).array<int>("a")[0]);
	}


	FACT("Object trees can be serialised")
	{
		Assert.Equal(convert(OBJECT_TREE), bson::to(tree().set("a", tree().set("b", 42)), false));
	}


	FACT("Object trees can be deserialised")
	{
		Assert.Equal(42, bson::from(convert(OBJECT_TREE)).get("a").get<int>("b"));
	}


	vector<tuple<vector<byte>>> invalid_bson()
	{
		return {
			make_tuple(vector<byte> { 0x00,0x00 }),																									// Document too short
			make_tuple(vector<byte> { 0x0f,0x00,0x00,0x00,0x00 }),																					// Invalid document length
			make_tuple(vector<byte> { 0x0d,0x00,0x00,0x00,0x00,0x10,0x61,0x00,0x2a,0x00,0x00,0x00,0x00 }), 											// Unexpected exit
			make_tuple(vector<byte> { 0x14,0x00,0x00,0x00,0x04,0x61,0x00,0x0c,0x00,0x00,0x00,0x10,0x31,0x00,0x2a,0x00,0x00,0x00,0x00,0x00 }),		// Invalid array index
			make_tuple(vector<byte> { 0x15,0x00,0x00,0x00,0x04,0x61,0x00,0x0d,0x00,0x00,0x00,0x00,0x10,0x31,0x00,0x2a,0x00,0x00,0x00,0x00,0x00 }),	// Unexpected array exit
			make_tuple(vector<byte> { 0x14,0x00,0x00,0x00,0x04,0x61,0x00,0x0f,0x00,0x00,0x00,0x10,0x30,0x00,0x2a,0x00,0x00,0x00,0x00,0x00 }),		// Invalid array document length

			make_tuple(vector<byte> { 0x0a,0x00,0x00,0x00,0x10,0x61,0x00,0x2a,0x00,0x00 }),							// Insufficient data to read int32
			make_tuple(vector<byte> { 0x0e,0x00,0x00,0x00,0x12,0x61,0x00,0x34,0x1c,0xdc,0xdf,0x02,0x00,0x00 }),		// Insufficient data to read int64
			make_tuple(vector<byte> { 0x0e,0x00,0x00,0x00,0x01,0x61,0x00,0x1f,0x85,0xeb,0x51,0xb8,0x1e,0x09 }),		// Insufficient data to read double
			make_tuple(vector<byte> { 0x0c,0x00,0x00,0x00,0x02,0x61,0x00,0x02,0x00,0x00,0x00,0x00 }),				// Insufficient data to read string
			make_tuple(vector<byte> { 0x0d,0x00,0x00,0x00,0x05,0x61,0x00,0x02,0x00,0x00,0x00,0x00,0x00 }),			// Insufficient data to read binary
			make_tuple(vector<byte> { 0x06,0x00,0x00,0x00,0x01,0x61 }),												// Unterminated cstring
		};
	}


	DATA_THEORY("Will throw exception if the BSON is invalid", (vector<byte> invalid), invalid_bson)
	{
		Assert.Throws<exception>([&](){ bson::from(convert(invalid)); });
	}
}