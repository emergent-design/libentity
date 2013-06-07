#include "entity/entity.h"

using namespace std;


namespace ent
{
	entity entity::get(const string name)
	{
		if (properties.count(name))
		{
			auto &p = this->properties[name];

			return p.type == vtype::Object ? *p.object : entity();
		}

		return entity();
	}


	vector<entity> entity::array(const string name)
	{
		vector<entity> result;

		if (this->properties.count(name))
		{
			auto &p = this->properties[name];

			if (p.type == vtype::Array)
			{
				for (auto &v : p.array)
				{
					if (v.type == vtype::Object) result.push_back(*v.object);
				}
			}
		}
		
		return result;
	}


	entity &entity::set(const string name, const entity &item)
	{
		this->properties[name] = value(make_shared<entity>(item));
		return *this;
	}



	// Store a binary vector as an encoded string
	entity &entity::set(const string name, const vector<byte> &item)
	{
		this->properties[name] = value(encode64(item));
		return *this;
	}


	entity &entity::set(const string name, const vector<entity> &items)
	{
		value result(vtype::Array);
		result.array.resize(items.size());

		transform(items.begin(), items.end(), result.array.begin(), [](const entity &v) {
			return value(make_shared<entity>(v));
		});

		this->properties[name] = result;

		return *this;
	}

}
