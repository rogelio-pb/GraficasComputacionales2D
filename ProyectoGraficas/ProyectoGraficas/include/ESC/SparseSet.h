#pragma once
#include "Prerequisites.h"
#include "ESC/Types.h"

namespace ESC {
	class
		SparseSet {
	public:
		SparseSet();
		virtual	~SparseSet() = default;

		//consultas
		[[nodiscard]] bool Contains(EntityID entity)const noexcept
		{
			const EntityIndex idx = GetEntityIndex(entity);
			if (idx >= m_sparse.size()) return false;
			const EntityIndex denseIdx = m_sparse[idx];
			return denseIdx < m_dense.size() && m_dense[denseIdx] == entity;
		}
		[[nodiscard]] size_t size() const noexcept { return m_dense.size(); }
		[[nodiscard]] bool empty() const noexcept { return m_dense.empty(); }

		[[nodiscard]] const std::vector<EntityID>& GetEntities() const noexcept
		{
			return m_dense;
		}

		//eliminacion de entidad (swap with last)
		virtual void Remove(EntityID entity)
		{
			if (!Contains(entity)) return;
			const EntityIndex sparseIdx = GetEntityIndex(entity);
			const EntityIndex denseIdx = m_sparse[sparseIdx];
			const EntityID last = m_dense.back();

			//mueve el ultimo elemento al hueco dejado por la entidad eliminada
			m_dense[denseIdx] = last;
			m_sparse[GetEntityIndex(last)] = denseIdx;
			m_dense.pop_back();

			//invalida la entrada eliminada
			m_sparse[sparseIdx] = INVALID;
		}

		virtual void Clear()
		{
			m_sparse.clear();
			m_dense.clear();
		}
	protected:
		//reserva espacio en m_sparse para al menos maxEntities entidades
		EntityIndex InsertEntity(EntityID entity)
		{
			const EntityIndex sparseIdx = GetEntityIndex(entity);
			const EntityIndex denseIdx = static_cast<EntityIndex>(m_dense.size());

			if (sparseIdx >= m_sparse.size())
				m_sparse.resize(sparseIdx + 1, INVALID);

			assert(m_sparse[sparseIdx] == INVALID && "La entidad ya esta en el set");

			m_sparse[sparseIdx] = denseIdx;
			m_dense.push_back(entity);
			return denseIdx;
		}
	

	protected:
		static constexpr EntityIndex INVALID = std::numeric_limits<EntityIndex>::max();

		std::vector<EntityIndex> m_sparse; //mapa de indice de entidad a indice en el array denso
		std::vector<EntityID> m_dense; //array de entidades activas
	};
}
