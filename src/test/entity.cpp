#include <xUnit++/xUnit++.h>
#include <entity/entity.h>
#include <entity/json.h>

using namespace std;
using namespace ent;


class SubClass : public entity
{
	public:
		string name	= "default";
		bool flag	= false;

		SubClass()
		{
			map("name", this->name);
			map("flag", this->flag);
		}
};

class TestClass : public entity
{
	public:

		string name;
		int integer;
		double floating;
		SubClass sub;

		vector<string> strings;
		vector<int> integers;
		vector<SubClass> objects;
		vector<byte> binary;

		TestClass()
		{
			map("name", this->name);
			map("integer", this->integer);
			map("floating", this->floating);
			map("sub", this->sub);
			map("strings", this->strings);
			map("integers", this->integers);
			map("objects", this->objects);
			map("binary", this->binary);
		}
};


string js = u8R"json(
{
	"binary": "AAECiP8=",
	"floating": 1.2,
	"integer": 42,
	"integers": [ 1, 1, 2, 3, 5, 8 ],
	"name": "hello",
	"objects": [
		{
			"flag": true,
			"name": "sub hello"
		},
		{
			"flag": true,
			"name": "sub hello"
		}
	],
	"strings": [
		"array",
		"of",
		"strings"
	],
	"sub": {
		"flag": true,
		"name": "sub hello"
	}
}
")json";


SUITE("Entity Tests")
{
	FACT("Mapping might work")
	{
		TestClass tc;

		/*tc.name = "hello";
		tc.integer = 42;
		tc.floating = 1.2;
		tc.sub.name = "sub hello";
		tc.sub.flag = true;

		tc.strings = { "array", "of", "strings" };
		tc.integers = { 1, 1, 2, 3, 5, 8 };
		tc.objects = { tc.sub, tc.sub };
		tc.binary = { 0x00, 0x01, 0x02, 0x88, 0xff };

		cout << tc.to<json>(true) << endl; //*/


		tc.from<json>(js);

		cout << tc.name << endl;
		cout << tc.integer << endl;
		cout << tc.floating << endl;
		cout << tc.sub.name << endl;
		cout << tc.sub.flag << endl;

		for (auto &i : tc.strings)	cout << i << ", ";		cout << endl;
		for (auto &i : tc.integers) cout << i << ", ";		cout << endl;
		for (auto &i : tc.objects)	cout << i.name << ", "; cout << endl;
		for (auto &i : tc.binary)	cout << (int)i << ", ";	cout << endl; //*/
	}
}


