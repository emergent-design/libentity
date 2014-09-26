#include <entity/json.h>
#include <entity/parser.hpp>
#include <chrono>
//#include <sstream>

#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>


using namespace std;
using namespace std::chrono;
using namespace ent;


struct Simple : entity
{
	string name		= "simple";
	bool flag		= true;
	int integer		= 42;
	long bignumber	= 20349758;
	double floating	= 3.142;

	mapping map()
	{
		return mapping() << eref(name) << eref(flag) << eref(integer) << eref(bignumber) << eref(floating);
	}

	template<class Archive> void serialize(Archive &archive)
	{
		archive(
			CEREAL_NVP(name),
			CEREAL_NVP(flag),
			CEREAL_NVP(integer),
			CEREAL_NVP(bignumber),
			CEREAL_NVP(floating)
		);
	}
};


struct Collection : entity
{
	std::vector<string> strings	 			= { "one", "two", "three" };
	std::vector<double> doubles				= { 0.11, 0.22, 0.33 };
	std::vector<byte> binary				= { 0x00, 0x01, 0x02, 0x88, 0xff };
	std::map<string, string> dictionary		= { { "first", "item" }, { "second", "item" } };

	mapping map()
	{
		return mapping() << eref(strings) << eref(doubles) << eref(binary) << eref(dictionary);
	}

	template<class Archive> void serialize(Archive &archive)
	{
		archive(
			CEREAL_NVP(strings),
			CEREAL_NVP(doubles),
			CEREAL_NVP(binary),
			CEREAL_NVP(dictionary)
		);
	}
};


struct Complex : entity
{
	string name 					= "complex";
	std::vector<Simple> entities	= { Simple(), Simple() };
	Collection collection;
	Simple simple;

	mapping map()
	{
		return mapping() << eref(name) << eref(entities) << eref(collection) << eref(simple);
	}

	template<class Archive> void serialize(Archive &archive)
	{
		archive(
			CEREAL_NVP(name),
			CEREAL_NVP(entities),
			CEREAL_NVP(collection),
			CEREAL_NVP(simple)
		);
	}
};


// #define BENCHMARK_ITERATIONS 1000000
#define BENCHMARK_ITERATIONS 100000
// #define BENCHMARK_ITERATIONS 1000


template <class T> void JsonTestTo(T &e, bool useCereal = false)
{
	string result;
	auto start = steady_clock::now();

	if (useCereal)
	{

		for (int i=0; i<BENCHMARK_ITERATIONS; i++)
		{
			//ss.str("");
			//ss.clear();
			stringstream ss;
 			cereal::JSONOutputArchive archive(ss);
 			archive(cereal::make_nvp("test", e));
 			result = ss.str();
 		}
	}
	else
	{
		for (int i=0; i<BENCHMARK_ITERATIONS; i++)
		{
			result = e.template to<json>();
		}
	}

	long duration = duration_cast<milliseconds>(steady_clock::now() - start).count();

	//cout << result << endl;
	cout << "Time taken for " << BENCHMARK_ITERATIONS << " iterations was " << duration << "ms" << endl;
	cout << "Which is an average of " << (1000.0 * (double)duration / BENCHMARK_ITERATIONS) << "us per serialisation" << endl;
}


#include <entity/json.hpp>
#include <entity/bson.hpp>

template <class T> void JsonTest2(T &e)
{
	string result;
	//json2 p;

	auto start = steady_clock::now();

	for (int i=0; i<BENCHMARK_ITERATIONS; i++)
	{
		result = entity2::encode<json2>(e); //p.to(e);
	}

	long duration = duration_cast<milliseconds>(steady_clock::now() - start).count();

	cout << "Time taken for " << BENCHMARK_ITERATIONS << " iterations was " << duration << "ms" << endl;
	cout << "Which is an average of " << (1000.0 * (double)duration / BENCHMARK_ITERATIONS) << "us per serialisation" << endl;
}

void JsonTestTo(tree &t)
{
	string result;

	auto start = steady_clock::now();
	for (int i=0; i<BENCHMARK_ITERATIONS; i++)
	{
		result = json::to(t, false);
	}

	long duration = duration_cast<milliseconds>(steady_clock::now() - start).count();

	cout << "Time taken for " << BENCHMARK_ITERATIONS << " iterations was " << duration << "ms" << endl;
	cout << "Which is an average of " << (1000.0 * (double)duration / BENCHMARK_ITERATIONS) << "us per serialisation" << endl;
}


void JsonTest2(tree2 &t)
{
	string result;

	auto start = steady_clock::now();
	for (int i=0; i<BENCHMARK_ITERATIONS; i++)
	{
		result = tree2::encode<json2>(t);
	}

	long duration = duration_cast<milliseconds>(steady_clock::now() - start).count();

	cout << "Time taken for " << BENCHMARK_ITERATIONS << " iterations was " << duration << "ms" << endl;
	cout << "Which is an average of " << (1000.0 * (double)duration / BENCHMARK_ITERATIONS) << "us per serialisation" << endl;
}


template <class T> void JsonTestFrom(T &e, string data, bool useCereal = false)
{
	auto start = steady_clock::now();

	if (useCereal)
	{
		stringstream ss(data);
		for (int i=0; i<BENCHMARK_ITERATIONS; i++)
		{
			ss.clear();
			ss.seekg(0,ios::beg);
			cereal::JSONInputArchive archive(ss);
       		archive(e);
		}
	}
	else
	{
		for (int i=0; i<BENCHMARK_ITERATIONS; i++)
		{
			e.template from<json>(data);
		}
	}

	long duration = duration_cast<milliseconds>(steady_clock::now() - start).count();

	//cout << e.name << endl;
	//cout << e << endl;
	cout << "Time taken for " << BENCHMARK_ITERATIONS << " iterations was " << duration << "ms" << endl;
	cout << "Which is an average of " << (1000.0 * (double)duration / BENCHMARK_ITERATIONS) << "us per deserialisation" << endl;
}

template <class T> void JsonTestFrom2(T &e, string data)
{
	auto start = steady_clock::now();

	for (int i=0; i<BENCHMARK_ITERATIONS; i++)
	{
		entity2::decode<json2>(data, e);
	}

	long duration = duration_cast<milliseconds>(steady_clock::now() - start).count();

	cout << "Time taken for " << BENCHMARK_ITERATIONS << " iterations was " << duration << "ms" << endl;
	cout << "Which is an average of " << (1000.0 * (double)duration / BENCHMARK_ITERATIONS) << "us per deserialisation" << endl;
}


void JsonTestFrom2(string data)
{
	tree2 result;
	auto start = steady_clock::now();

	for (int i=0; i<BENCHMARK_ITERATIONS; i++)
	{
		// result = tree2::decode<json2>(data);
		tree2::decode<json2>(data);
	}

	long duration = duration_cast<milliseconds>(steady_clock::now() - start).count();

	cout << "Time taken for " << BENCHMARK_ITERATIONS << " iterations was " << duration << "ms" << endl;
	cout << "Which is an average of " << (1000.0 * (double)duration / BENCHMARK_ITERATIONS) << "us per deserialisation" << endl;
}


void JsonTestFrom(string data)
{
	tree result;

	auto start = steady_clock::now();
	for (int i=0; i<BENCHMARK_ITERATIONS; i++)
	{
		result = json::from(data);
	}

	long duration = duration_cast<milliseconds>(steady_clock::now() - start).count();

	cout << "Time taken for " << BENCHMARK_ITERATIONS << " iterations was " << duration << "ms" << endl;
	cout << "Which is an average of " << (1000.0 * (double)duration / BENCHMARK_ITERATIONS) << "us per deserialisation" << endl;
}


#include <entity/entity.hpp>

enum class MyEnum { First, Second };

struct Simple2 : entity2
{
	string name		= "simple";
	bool flag		= true;
	int integer		= 42;
	long bignumber	= 20349758;
	double floating	= 3.142;
	MyEnum myenum	= MyEnum::Second;

	mapping2 map()
	{
		return mapping2() << eref2(name) << eref2(flag) << eref2(integer) << eref2(bignumber) << eref2(floating) << eref2(myenum);
	}
};

struct Collection2 : entity2
{
	std::vector<string> strings	 			= { "one", "two", "three" };
	std::vector<double> doubles				= { 0.11, 0.22, 0.33 };
	std::vector<byte> binary				= { 0x00, 0x01, 0x02, 0x88, 0xff };
	std::map<string, string> dictionary		= { { "first", "item" }, { "second", "item" } };

	//std::map<string, vector<Simple2>> arraymap	= {{ "array", { {}, {} } }};
	//vector<std::map<string, string>> maparray	= { { { "first", "item" }, { "second", "item" }}, { {"another", "map"}} };

	mapping2 map()
	{
		return mapping2() << eref2(strings) << eref2(doubles) << eref2(binary) << eref2(dictionary);// << eref2(arraymap) << eref2(maparray);
	}
};


struct Complex2 : entity2
{
	string name 					= "complex";
	std::vector<Simple2> entities	= { Simple2(), Simple2() };
	Collection2 collection;
	Simple2 simple;

	mapping2 map()
	{
		return mapping2() << eref2(name) << eref2(entities) << eref2(collection) << eref2(simple);
	}
};



int main(int argc, char **argv)
{
	string simpleData		= R"json({"bignumber":20349758,"flag":true,"floating":3.142,"integer":42,"name":"simple"})json";
	string simpleCereal		= R"json({"test":{"bignumber":20349758,"flag":true,"floating":3.142,"integer":42,"name":"simple"}})json";
	string complexData		= R"json({"collection":{"binary":"AAECiP8=","dictionary":{"first":"item","second":"item"},"doubles":[0.11,0.22,0.33],"strings":["one","two","three"]},"entities":[{"bignumber":20349758,"flag":true,"floating":3.142,"integer":42,"name":"simple"},{"bignumber":20349758,"flag":true,"floating":3.142,"integer":42,"name":"simple"}],"name":"complex","simple":{"bignumber":20349758,"flag":true,"floating":3.142,"integer":42,"name":"simple"}})json";
	string complexCereal	= R"json({"test":{"collection":{"binary":[0,1,2,136,255],"dictionary":[{"key":"first","value":"item"},{"key":"second","value":"item"}],"doubles":[0.11,0.22,0.33],"strings":["one","two","three"]},"entities":[{"bignumber":20349758,"flag":true,"floating":3.142,"integer":42,"name":"simple"},{"bignumber":20349758,"flag":true,"floating":3.142,"integer":42,"name":"simple"}],"name":"complex","simple":{"bignumber":20349758,"flag":true,"floating":3.142,"integer":42,"name":"simple"}}})json";


	tree2 t = {
		{ "first", 42 },
		{ "second", true },
		{ "third", "forty-two" },
		{ "fourth", 5.6 },
		{ "child", {
			{ "first", "name"}
		}},
		{ "array", vector<tree2> { 42, true, "hello"}}
	};

	// cout << t["first"].as_long(10) << endl;
	// cout << t["doesn't exist"].as_long(10) << endl;
	// for (auto &i : t["array"].as_array()) cout << i.as_string() << endl;

	//cout << encode_tree<prettyjson>(t) << endl;

	//auto a = tree2::decode<json2>(complexData);

	//cout << tree2::encode<prettyjson>(a) << endl;

	Simple2 simplex;
	Complex2 complexi;
	Collection2 collectioni;

	//cout << entity2::encode<json2>(simplex) << endl;
	// cout << entity2::encode<prettyjson>(complexi) << endl;
	cout << entity2::encode<prettyjson>(collectioni) << endl;

	//cout << a["collection"]["dictionary"]["first"].as_string() << endl;

	return 0; //*/



	Simple simple;
	Complex complex;
	Simple2 simple2;
	Complex2 complex2;
	//Collection2 collection;

	//json2 p;

	//cout << p.to(simple, true) << endl;
	//cout << p.to(collection, true) << endl;
	//cout << p.to(complex, true) << endl;

	//cout << json2::jcodec::to(complex, true) << endl;

	//Complex complex;

	// JsonTest2(complex2);
	// JsonTestTo(complex);
	// JsonTestTo(complex, true);
	// JsonTest2(complex2);

	// cout << "New: " << endl;	JsonTestFrom2(simple2, simpleData);
	// cout << "Old: " << endl;	JsonTestFrom(simple, simpleData);
	// cout << "Cereal: " << endl;	JsonTestFrom(simple, simpleCereal, true);
	// cout << "New: " << endl;	JsonTestFrom2(simple2, simpleData);
	//

	cout << "New: " << endl;	JsonTestFrom2(complex2, complexData);
	cout << "Old: " << endl;	JsonTestFrom(complex, complexData);
	cout << "Cereal: " << endl;	JsonTestFrom(complex, complexCereal, true);
	cout << "New: " << endl;	JsonTestFrom2(complex2, complexData);
	cout << "New tree: " << endl;	JsonTestFrom2(complexData);

	// JsonTestTo(complex);
	// JsonTestTo(complex, true);
	// JsonTest2(complex2);

	// cout << encode<json2pretty>(complex2) << endl;

	//cout << encode<bson2>(complex2) << endl;

	complex2 = entity2::decode<json2, Complex2>(complexData);
	cout << entity2::encode<prettyjson>(complex2) << endl;

	// simple2 = decode<json2, Simple2>(simpleData);
	// cout << encode<json2pretty>(simple2) << endl;

//whitespace[' '] = whitespace['\t'] = whitespace['\r'] = whitespace['\n'] = whitespace[','] = true;
	//cout << (int)' ' << ", " << (int)'\t' << ", " << (int)'\r' << ", " << (int)'\n' << ", " << (int)',' << endl;


	return 0;


/*	string which 		= (argc > 1) ? argv[1] : "simple";	// simple/complex
	string layer		= (argc > 2) ? argv[2] : "entity";	// entity/tree/cereal
	string type			= (argc > 3) ? argv[3] : "json";	// json
	string direction	= (argc > 4) ? argv[4] : "to";		// to/from

	Simple simple;
	Complex complex;
	string simpleData		= R"json({"bignumber":20349758,"flag":true,"floating":3.142,"integer":42,"name":"simple"})json";
	string simpleCereal		= R"json({"test":{"bignumber":20349758,"flag":true,"floating":3.142,"integer":42,"name":"simple"}})json";
	string complexData		= R"json({"collection":{"binary":"AAECiP8=","dictionary":{"first":"item","second":"item"},"doubles":[0.11,0.22,0.33],"strings":["one","two","three"]},"entities":[{"bignumber":20349758,"flag":true,"floating":3.142,"integer":42,"name":"simple"},{"bignumber":20349758,"flag":true,"floating":3.142,"integer":42,"name":"simple"}],"name":"complex","simple":{"bignumber":20349758,"flag":true,"floating":3.142,"integer":42,"name":"simple"}})json";
	string complexCereal	= R"json({"test":{"collection":{"binary":[0,1,2,136,255],"dictionary":[{"key":"first","value":"item"},{"key":"second","value":"item"}],"doubles":[0.11,0.22,0.33],"strings":["one","two","three"]},"entities":[{"bignumber":20349758,"flag":true,"floating":3.142,"integer":42,"name":"simple"},{"bignumber":20349758,"flag":true,"floating":3.142,"integer":42,"name":"simple"}],"name":"complex","simple":{"bignumber":20349758,"flag":true,"floating":3.142,"integer":42,"name":"simple"}}})json";
	string data				= which == "complex" ? complexData : simpleData;
	tree object				= which == "complex" ? complex.to() : simple.to();

	if (layer == "cereal") data = which == "complex" ? complexCereal : simpleCereal;

	if (type == "json")
	{
		if (direction == "to")
		{
			if (layer == "tree")			JsonTestTo(object);
			else if (which == "complex")	JsonTestTo(complex, layer == "cereal");
			else							JsonTestTo(simple, layer == "cereal");
		}
		else
		{
			if (layer == "tree")			JsonTestFrom(data);
			else if (which == "complex")	JsonTestFrom(complex, data, layer == "cereal");
			else							JsonTestFrom(simple, data, layer == "cereal");
		}
	}
	else cout << "Unknown test" << endl;

	return 0;*/
}
