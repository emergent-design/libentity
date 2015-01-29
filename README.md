libentity
=========

Entity is a C++11 library that provides object serialisation/deserialisation
yet requires very little boilerplate code.

It supports the serialisation of not only structs and classes but also `tree`
which is a container class that supports dynamic storage of strings, numeric
values, booleans, binary blobs, vectors and objects (other trees).


Status
------

This library has been used in a complex production system for some time now and
we consider it stable.


Codecs
------

The [codecs](https://github.com/emergent-design/libentity/wiki/Codecs) implemented
so far:

* JSON
* BSON


Example
-------

```cpp
#include <iostream>
#include <entity/entity.hpp>
#include <entity/json.hpp>

using namespace std;
using namespace ent;


// A structure with simple members, derived from entity
struct Simple : entity
{
	string name	= "default";
	bool flag	= false;
	int integer = 0;

	mapping describe()
	{
		return { eref(name), eref(flag), eref("int", integer) };
	}
};


const string JSON_TEXT = u8R"json(
	{
		"name": "simple",
		"flag": true,
		"int":  42
	}
)json";


int main(int argc, char *argv[])
{
	Simple simple;

	// Serialise the simple object to JSON (will contain the default
	// values assigned above)
	cout << entity::encode<json>(simple) << endl;

	// Deserialise the simple object from a JSON string
	simple = entity::decode<json>(JSON_TEXT);

	// These values will now match those defined in JSON_TEXT
	cout << "Name    = " << simple.name << endl;
	cout << "Flag    = " << simple.flag << endl;
	cout << "Integer = " << simple.integer << endl;

	return 0;
}
```

If the above example was saved to "simple.cpp" then you would compile it as follows

```bash
clang -std=c++11 simple.cpp
```

Any class or struct that descends from
[entity](https://github.com/emergent-design/libentity/wiki/Entity)
must implement the
[describe](https://github.com/emergent-design/libentity/wiki/Entity#describe)
function which allows the library to do its magic without the need for a
pre-compilation step. The macro ```eref``` simply expands to a
mapping entry and if no name is provided it will automagically use the parameter
name (so avoid using ```this->```).
By allowing the name to be specified, as in the "int" example above, it can map a
value from a third-party system where the names may differ.


Installation
------------

Entity is a header-only library, so simply include the entity headers in your
project or copy them to somewhere on the include path (such as ```/usr/local/include```).
Alternatively you could [download](http://downloads.emergent-design.co.uk/libentity)
a package for any recent debian-based distribution which will install the headers
to ```/usr/include```.


Requirements
------------

A modern compiler that supports C++11 features. It should work with any recent version of g++, clang or msvc.


---

<div style="text-align:center">
	<a href="https://github.com/emergent-design/libentity/blob/master/packages/debian/changelog">
		Changelog
	</a>
</div>
