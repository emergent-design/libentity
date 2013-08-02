#include "entity/tree.h"

using namespace std;


namespace ent
{
	tree tree::get(const string name) const
	{
		return properties.count(name) ? this->properties.at(name).object() : tree();
	}


	vector<tree> tree::array(const string name) const
	{
		vector<tree> result;

		if (this->properties.count(name))
		{
			auto &p = this->properties.at(name);

			if (p.get_type() == value::Type::Array)
			{
				for (auto &v : p.array())
				{
					if (v.get_type() == value::Type::Object) result.push_back(v.object());
				}
			}
		}

		return result;
	}


	tree &tree::set(const std::string name)
	{
		this->properties[name] = value();
		return *this;
	}


	tree &tree::set(const string name, const tree &item)
	{
		this->properties[name] = value(make_shared<tree>(item));
		return *this;
	}


	tree &tree::set(const string name, const vector<byte> &item)
	{
		this->properties[name] = value(item);
		return *this;
	}


	tree &tree::set(const string name, const vector<tree> &items)
	{
		vector<value> result(items.size());

		transform(items.begin(), items.end(), result.begin(), [](const tree &v) {
			return value(make_shared<tree>(v));
		});

		this->properties[name] = value(result);

		return *this;
	}

}
