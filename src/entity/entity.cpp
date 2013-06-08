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


	template <> void entity::map(std::string name, std::vector<byte> &reference)
	{
		this->mapping[name] = std::make_shared<vmap<vector<byte>>>(reference);
	}
}
