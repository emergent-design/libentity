#include "entity/value.h"
#include "entity/entity.h"


using namespace std;


namespace ent
{
	bool value::null() { return this->type == vtype::Null; }


	std::string value::get(std::string defaultValue)	{ return this->type == vtype::String	? this->string : defaultValue; }
	float value::get(float defaultValue)				{ return this->type == vtype::Number	? this->number : defaultValue; }
	double value::get(double defaultValue)				{ return this->type == vtype::Number	? this->number : defaultValue; }
	int value::get(int defaultValue)					{ return this->type == vtype::Number	? lrint(this->number) : defaultValue; }
	long value::get(long defaultValue)					{ return this->type == vtype::Number	? lrint(this->number) : defaultValue; }
	bool value::get(bool defaultValue)					{ return this->type == vtype::Boolean	? this->boolean : defaultValue; }
	tree value::get(tree defaultValue)					{ return this->type == vtype::Object	? *this->object : defaultValue; }


	std::vector<byte> value::get(std::vector<byte> defaultValue)
	{
		return this->type == vtype::String ? decode64(this->string) : defaultValue;
	}
	

	template <> bool value::is<std::string>()	{ return this->type == vtype::String; }
	template <> bool value::is<float>()			{ return this->type == vtype::Number; }
	template <> bool value::is<double>()		{ return this->type == vtype::Number; }
	template <> bool value::is<int>()			{ return this->type == vtype::Number; }
	template <> bool value::is<long>()			{ return this->type == vtype::Number; }
	template <> bool value::is<bool>()			{ return this->type == vtype::Boolean; }
	template <> bool value::is<tree>()			{ return this->type == vtype::Object; }
}
