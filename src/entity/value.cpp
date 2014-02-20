#include "entity/value.h"
#include "entity/entity.h"


using namespace std;


namespace ent
{
	bool value::null() const					{ return this->type == Type::Null; }
	value::Type value::get_type() const			{ return this->type; }
	value::Number value::get_numtype() const	{ return this->number; }
	bool value::is_floating() const 			{ return this->type == Type::Number && this->number == Number::Floating; }


	value &value::operator=(const value &v)
	{
		delete this->content;
		this->type 		= v.type;
		this->number	= v.number;
		this->content 	= v.content ? v.content->clone() : nullptr;
		return *this;
	}


	std::string value::get(const std::string &defaultValue) const
	{
		return this->type == Type::String ? static_cast<container<std::string> *>(this->content)->data : defaultValue;
	}


	bool value::get(const bool &defaultValue) const
	{
		return this->type == Type::Boolean ? static_cast<container<bool> *>(this->content)->data : defaultValue;
	}


	tree value::get(const tree &defaultValue) const
	{
		return this->type == Type::Object ? this->object() : defaultValue;
	}


	vector<value> &value::array() const
	{
		if (this->type != Type::Array) throw runtime_error("Attempting to access non-array type as array");

		return static_cast<container<vector<value>> *>(this->content)->data;
	}


	tree &value::object() const
	{
		if (this->type != Type::Object) throw runtime_error("Attempting to access non-object type as object");

		return *static_cast<container<shared_ptr<tree>> *>(this->content)->data;
	}


	vector<byte> value::get(const vector<byte> &defaultValue) const
	{
		return this->type == Type::Binary
			? static_cast<container<vector<byte>> *>(this->content)->data
			: this->type == Type::String
				? decode64(static_cast<container<std::string> *>(this->content)->data)
				: defaultValue;
	}
}
