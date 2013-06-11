#include "entity/tree.h"

using namespace std;


namespace ent
{
	tree tree::get(const string name)
	{
		if (properties.count(name))
		{
			auto &p = this->properties[name];

			return p.type == vtype::Object ? *p.object : tree();
		}

		return tree();
	}


	vector<tree> tree::array(const string name)
	{
		vector<tree> result;

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


	tree &tree::set(const string name, const tree &item)
	{
		this->properties[name] = value(make_shared<tree>(item));
		return *this;
	}


	tree &tree::set(const string name, const vector<byte> &item)
	{
		this->properties[name] = value(encode64(item));
		return *this;
	}


	tree &tree::set(const string name, const vector<tree> &items)
	{
		value result(vtype::Array);
		result.array.resize(items.size());

		transform(items.begin(), items.end(), result.array.begin(), [](const tree &v) {
			return value(make_shared<tree>(v));
		});

		this->properties[name] = result;

		return *this;
	}

}
