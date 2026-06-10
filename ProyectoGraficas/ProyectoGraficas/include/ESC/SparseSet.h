#pragma once
#include "Prerequisites.h"
#include "ESC/Types.h"

namespace ESC {
	class
		SparseSet {
	public:
		SparseSet();
		virtual	~SparseSet() = default;


 /**
 * @brief Comprueba si una entidad está en el set.
 * @param entity Entidad a buscar.
 * @return true si existe.
 */
		//consultas
		[[nodiscard]] bool Contains(EntityID entity)const noexcept
		{
			const EntityIndex idx = GetEntityIndex(entity);
			if (idx >= m_sparse.size()) return false;
			const EntityIndex denseIdx = m_sparse[idx];
			return denseIdx < m_dense.size() && m_dense[denseIdx] == entity;
		}
/**
  * @brief Cantidad de entidades almacenadas.
  * @return Número de entidades.
  */
		[[nodiscard]] size_t size() const noexcept { return m_dense.size(); }
 /**
 * @brief Comprueba si el set está vacío.
 * @return true si no hay entidades.
 */
		[[nodiscard]] bool empty() const noexcept { return m_dense.empty(); }

		[[nodiscard]] const std::vector<EntityID>& GetEntities() const noexcept
		{
			return m_dense;
		}

	  /**
	  * @brief Elimina una entidad del set.
	  * @param entity Entidad a eliminar.
	  */
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
 /**
 * @brief Inserta una entidad en el set.
 * @param entity Entidad a agregar.
 * @return Índice dentro del arreglo denso.
 */
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
