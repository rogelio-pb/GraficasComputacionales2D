#pragma once
#include "Prerequisites.h"
#include "Esc/ComponentPool.h"

namespace ECS {
	template<typename... Components>
	class View {
	public:
  /**
  * @brief Constructor de la vista
  * @param pools Pools de los componentes a consulta
  */
		explicit View(ComponentPool<Components>*... pools) noexcept
			: m_pools(pools...) {
			FinSmallest();
		}

		//Iterador para recorrer las entidades que tienen todos los componentes del view
		template<typename Func>
		void Each(Func&& func){
			if (!m_smallest) return;
			const auto& entities = m_smallest->GetEntities();

			//recorrido inverso para eliminar entidades sin invalidar el iterador
			for (std::size_t i = entities.size(); i > 0; --i)
			{
				const EntityID entity = entities[i - 1];
				if (AllHave(entity))
				{
					std::apply(
						[&](auto*... pools) {
							func(entity, pools->Get(entity)...);

						},
						m_pools);
				}
			}
		}
		/**
	* @brief Rrcorre solamente las entidades
	* @param func funcion a ejecutar
	*/

		//iteracion solo de entidades
		template<typename Func>
		void EachEntity(Func&& func)
		{
			if (!m_smallest)return;
			const auto& entities = m_smallest->GetEntities();
			for (std::size_t i = entities.size(); i > 0; --i)
			{
				const EntityID entity = entities[i - 1];
				if (AllHave(entity))
					func(entity);
			}
		}
		[[nodiscard]] bool Empty() const noexcept { return !m_smallest || m_smallest->Empty(); }
		[[nodiscard]] std::size_t Size() const noexcept { return m_smallest ? m_smallest->Size() : 0; }

	private:
		/**
   * @brief Busca el pool con menos entidads
   * se usa para hacer la iteración más rapdda
   */
		template < std::size_t I = 0> 
		void FinSmallest() noexcept {
			if constexpr (I < sizeof...(Components)) {
				auto* pool = std::get<I>(m_pools);
				if (pool && (!m_smallest || pool->Size() < m_smallest->Size()))
					m_smallest = pool;
				FinSmallest<I + 1>();
			}
		}
		[[nodiscard]] bool AllHave(EntityID entity) const noexcept
		{
			return std::apply(
				[entity](auto*... pools) noexcept {
					return (... &&(pools && pools->Contains(entity)));
				},
				m_pools);
		}

	private:
		std::tuple<ComponentPool<Components>*...>m_pools;
		/**
	* @brief Pool mas pequeño encontrado
	* Se usa para iterarmenos entidades y mejorar rendimiento
	*/
		const SparseSet* m_smallest = nullptr; //puntero al SparseSet del pool con menos entidades, para iterar eficientemente
		
		
	};
}
