#include <xUnit++/xUnit++.h>
#include <entity/entity.h>
#include <entity/xml.h>

using namespace std;
using namespace ent;


const string HEADER_XML = u8R"xml(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>)xml";
const string PADDED_XML = u8R"xml(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<entity>
  <flag value="true" />
  <integer value="42" />
  <name>simple</name>
</entity>)xml";


SUITE("XML Tests")
{
	FACT("Will skip the XML header if it exists")
	{
		auto t = xml::from(u8R"xml(<?xml version="1.0" encoding="UTF-8" standalone="yes"?><entity><name>simple</name>)xml");

		Assert.Equal("simple", t.get<string>("name"));
	}


	FACT("Compact XML can be generated")
	{
		auto t = tree().set("name", "simple").set("integer", 42).set("flag", true);

		Assert.Equal(HEADER_XML + u8R"xml(<entity><flag value="true" /><integer value="42" /><name>simple</name></entity>)xml", xml::to(t, false));
	}


	FACT("Compact XML can be parsed")
	{
		auto t = xml::from(u8R"xml(<entity><flag value="true" /><integer value="42" /><name>simple</name></entity>)xml");

		Assert.Equal("simple", t.get<string>("name"));
		Assert.Equal(42, t.get<int>("integer"));
		Assert.True(t.get<bool>("flag"));
	}


	FACT("Padded XML can be generated")
	{
		auto t = tree().set("name", "simple").set("integer", 42).set("flag", true);

		Assert.Equal(PADDED_XML, xml::to(t, true));
	}


	FACT("Padded XML can be parsed")
	{
		auto t = xml::from(u8R"xml(
			<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
			<entity>
				<flag value="true" />
				<integer value="42" />
				<name>simple</name>
			</entity>
		)xml");

		Assert.Equal("simple", t.get<string>("name"));
		Assert.Equal(42, t.get<int>("integer"));
		Assert.True(t.get<bool>("flag"));
	}


	FACT("Strings are escaped appropriately")
	{
		auto t = tree().set("text", "<This> needs & escaping \\ \"properly\"");

		Assert.Equal(HEADER_XML + u8R"xml(<entity><text>&lt;This&gt; needs &amp; escaping \ &quot;properly&quot;</text></entity>)xml", xml::to(t, false));
	}


	FACT("Strings are unescaped appropriately")
	{
		auto t = xml::from(u8R"xml(
			<entity>
				<text>&lt;This&gt; needs &amp; unescaping \ &quot;properly&quot;</text>
			</entity>
		)xml");

		Assert.Equal("<This> needs & unescaping \\ \"properly\"", t.get<string>("text"));
	}


	FACT("Simple types can be parsed")
	{
		auto t = xml::from(u8R"xml(
			<entity>
				<name>simple</name>
				<integer value="42" />
				<double value="3.14" />
				<flag value="true" />
				<nothing value="null" />
			</entity>
		)xml");

		Assert.Equal(vtype::String,		t.properties["name"].type);
		Assert.Equal(vtype::Number,		t.properties["integer"].type);
		Assert.Equal(vtype::Number,		t.properties["double"].type);
		Assert.Equal(vtype::Boolean,	t.properties["flag"].type);
		Assert.Equal(vtype::Null,		t.properties["nothing"].type);
	}


	FACT("Arrays can be parsed")
	{
		auto t = xml::from(u8R"xml(
			<entity>
				<array>
					<_ value="1" />
					<_ value="2" />
					<_ value="3" />
					<_ value="4" />
				</array>
			</entity>
		)xml");

		Assert.Equal(vtype::Array,	t.properties["array"].type);
		Assert.Equal(4,				t.properties["array"].array.size());
	}


	FACT("Object trees can be parsed")
	{
		auto t = xml::from(u8R"xml(
			<entity>
				<object>
					<name>complex</name>
				</object>
			</entity>
		)xml");

		Assert.Equal(vtype::Object,	t.properties["object"].type);
		Assert.Equal("complex",		t.properties["object"].object->get<string>("name"));
	}


	FACT("Can cope with an empty object")
	{
		Assert.Equal(0, xml::from(u8R"xml(<entity></entity>)xml").properties.size());
	}


	FACT("Can cope with alternative style line endings")
	{
		Assert.Equal(2, xml::from("<entity><a value=\"1\" />\r\n<b value=\"2\" /></entity>").get<int>("b"));
	}


	FACT("Can cope with standard JSON number formats")
	{
		auto t = xml::from(u8R"xml(
			<entity>
				<integer value="42" />
				<double value="3.14" />
				<scientific value="3.141e-10" />
				<upper value="3.141E-10" />
				<long value="12345123456789" />
				<big value="123456789123456789123456789" />
			</entity>
		)xml");

		Assert.Equal(42,							t.get<int>("integer"));
		Assert.Equal(3.14,							t.get<double>("double"));
		Assert.Equal(3.141e-10,						t.get<double>("scientific"));
		Assert.Equal(3.141e-10,						t.get<double>("upper"));
		Assert.Equal(12345123456789,				t.get<long>("long"));
		Assert.Equal(123456789123456789123456789.0,	t.get<double>("big"));
	}


	vector<tuple<string>> invalid_xml()
	{
		return {
			make_tuple(u8R"xml(<entity></etnity>)xml"),							// Mismatched top-level tag
			make_tuple(u8R"xml(<entity><a></b></entity>)xml"),					// Mismatched tag
			make_tuple(u8R"xml(<entity>something</entity>)xml"),				// Missing start tag
			make_tuple(u8R"xml(<entity><a><b>text</a></entity>)xml"),			// Missing end tag
			make_tuple(u8R"xml(<entity><a value="42"></entity>)xml"),			// Incorrectly closed single tag
			make_tuple(u8R"xml(<entity><a value=42 /></entity>)xml"),			// Missing value quotes
			make_tuple(u8R"xml(<entity><a value "42" /></entity>)xml"),			// Missing value assignment symbol
			make_tuple(u8R"xml(<entity><a something="else" /></entity>)xml"),	// Unknown attribute
			make_tuple(u8R"xml(<entity><a><_>1</_><b>2</b></a></entity>)xml"),	// Non-item inside array
			make_tuple(u8R"xml(<entity><a value="invalid" /></entity>)xml")		// Invalid value type
		};
	}


	DATA_THEORY("Will throw exception if the XML is invalid", (string invalid), invalid_xml)
	{
		Assert.Throws<exception>([&](){ xml::from(invalid); });
	}
}
