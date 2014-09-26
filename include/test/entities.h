#pragma once
#include <entity/entity.hpp>


struct TextEntity : ent::entity2
{
	std::string text = "";

	ent::mapping2 map()
	{
		return ent::mapping2() << eref2(text);
	}
};


struct SimpleEntity : ent::entity2
{
	std::string name	= "simple";
	bool flag			= true;
	int integer			= 42;
	long bignumber		= 20349758;
	double floating		= 3.142;

	ent::mapping2 map()
	{
		return ent::mapping2() << eref2(name) << eref2(flag) << eref2(integer) << eref2(bignumber) << eref2(floating);
	}
};


struct CollectionEntity : ent::entity2
{
	std::vector<std::string> strings;//	 			= { "one", "two", "three" };
	std::vector<double> doubles;//						= { 0.11, 0.22, 0.33 };
	std::vector<byte> binary;//						= { 0x00, 0x01, 0x02, 0x88, 0xff };
	std::map<std::string, std::string> dictionary;//	= { { "first", "item" }, { "second", "item" } };

	ent::mapping2 map()
	{
		return ent::mapping2() << eref2(strings) << eref2(doubles) << eref2(binary) << eref2(dictionary);
	}
};


struct ComplexEntity : ent::entity2
{
	std::string name; // 					= "complex";
	std::vector<SimpleEntity> entities; //	= { SimpleEntity(), SimpleEntity() };
	CollectionEntity collection;
	SimpleEntity simple;

	ent::mapping2 map()
	{
		return ent::mapping2() << eref2(name) << eref2(entities) << eref2(collection) << eref2(simple);
	}
};
