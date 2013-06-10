#include "entity/entity.h"

using namespace std;


namespace ent
{
	tree entity::to_tree()
	{
		tree result;

		// Refresh the mappings every time since we can't rely on doing it in
		// the entity constructor since a copy construct would actually copy
		// the mappings as well thereby using references to another object
		// instance (that potentially no longer exists).
		this->create_map();

		for (auto &m : this->mapping)
		{
			result.properties[m.first] = m.second->to();
		}

		return result;
	}


	void entity::from_tree(tree &tree)
	{
		// Refresh the mappings (see above)
		this->create_map();
		
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
