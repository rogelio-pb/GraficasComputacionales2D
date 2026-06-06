#pragma once
#include "ESC/Types.h"
#include "ESC/ComponentPool.h"
#include"ESC/View.h"

namespace ESC {
	class Registry {
	private:
		// entidades
		std::vector<EntityID>		m_entities;
		std::vector<EntityVersion>	m_versions;
		std::queue<EntityIndex>		m_freelist;

		std::unordered_map<ComponentTypeID, std::unique_ptr<IComonentPool>> m_componentPools;
		 //sistemas 
		std::vector<std::unique_ptr<System>>m_sistem;
	};
}
