#include "entity/entity.h"

using namespace std;


namespace ent
{
	tree entity::to_tree()
	{
		tree result;

		for (auto &m : this->mapping)
		{
			result.properties[m.first] = m.second->to();
		}

		return result;
	}


	void entity::from_tree(tree &tree)
	{
		for (auto &m : this->mapping)
		{
			m.second->from(tree.properties[m.first]);
		}
	}
}
