#include "entity/mapping.h"

using namespace std;


namespace ent
{
	mapping::mapping(const reference &item)
	{
		*this << item;
	}


	mapping::mapping(std::initializer_list<reference> items)
	{
		*this << items;
	}


	mapping &mapping::operator<<(const reference &item)
	{
		this->lookup[item.name] = item.item;
		return *this;
	}


	mapping &mapping::operator<<(std::initializer_list<reference> items)
	{
		for (auto &i : items) this->lookup[i.name] = i.item;
		return *this;
	}


	tree mapping::to()
	{
		tree result;

		for (auto &i : this->lookup)
		{
			result.properties[i.first] = i.second->to();
		}

		return result;
	}


	void mapping::from(const tree &tree)
	{
		for (auto &i : this->lookup)
		{
			i.second->from(tree.properties.count(i.first) ? tree.properties.at(i.first) : value());
		}
	}
}
