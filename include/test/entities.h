#pragma once
#include <entity/entity.hpp>


struct TextEntity : ent::entity
{
	std::string text = "";

	ent::mapping describe()
	{
		return { eref(text) };
	}
};


struct SimpleEntity : ent::entity
{
	std::string name	= "simple";
	bool flag			= true;
	int integer			= 42;
	long bignumber		= 20349758;
	double floating		= 3.142;

	ent::mapping describe()
	{
		return { eref(name), eref(flag), eref(integer), eref(bignumber), eref(floating) };
	}
};


struct CollectionEntity : ent::entity
{
	std::vector<std::string> strings;//	 			= { "one", "two", "three" };
	std::vector<double> doubles;//						= { 0.11, 0.22, 0.33 };
	std::vector<byte> binary;//						= { 0x00, 0x01, 0x02, 0x88, 0xff };
	std::map<std::string, std::string> dictionary;//	= { { "first", "item" }, { "second", "item" } };

	ent::mapping describe()
	{
		return { eref(strings), eref(doubles), eref(binary), eref(dictionary) };
	}
};


struct ComplexEntity : ent::entity
{
	std::string name; // 					= "complex";
	std::vector<SimpleEntity> entities; //	= { SimpleEntity(), SimpleEntity() };
	CollectionEntity collection;
	SimpleEntity simple;

	ent::mapping describe()
	{
		return { eref(name), eref(entities), eref(collection), eref(simple) };
	}
};
