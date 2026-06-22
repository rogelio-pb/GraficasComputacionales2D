#pragma once
#include "ESC/Types.h"
#include "ESC/ComponentPool.h"
#include "ESC/System.h"
#include"ESC/View.h"

namespace ECS {
	class Registry {
	public:
		/**
 * @brief Crea una entidad nueva.
 * @return ID de la entidad creada.
 */
		EntityID CreateEntity() {
			EntityIndex idx;
			if (!m_freeList.empty()) {
				idx = m_freeList.front();
				m_freeList.pop();
			}
			else {
				idx = static_cast<EntityIndex>(m_entities.size());
				m_entities.push_back(0); //placeholder, se actualizara con el ID completo
				m_versions.push_back(NULL_ENTITY); //version inicial
			}
			EntityID id = MakeEntityID(idx, m_versions[idx]);
			m_entities[idx] = id;
			return id;
		}
		/**
	* @brief Elimina una entidad y sus componentes.
	* @param entity Entidad a destruir.
	*/
		void
			DestroyEntity(EntityID entity) {

		  assert(IsAlive(entity) && "Intentando destruir una entidad no valida");

			for (auto& [typeID, pool] : m_componentPools) {
				pool->RemoveEntity(entity);
			}
			const EntityIndex idx = GetEntityIndex(entity);
			++m_versions[idx]; //incrementa la version para invalidar IDs antiguos
			m_entities[idx] = NULL_ENTITY;
			m_freeList.push(idx);
		}
		/**
				 * @brief Revisa si una entidad sigue existiendo.
				 * @param entity Entidad a comprobar.
				 * @return true si existe.
				 */
		[[nodiscard]] bool
			IsAlive(EntityID entity) const noexcept {
			const EntityIndex idx = GetEntityIndex(entity);
			return idx < m_entities.size() && m_entities[idx] == entity;
		}

		[[nodiscard]] std::size_t
			EntityCount() const noexcept {
			return m_entities.size() - m_freeList.size();
		}

		[[nodiscard]] const std::vector<EntityID>&
			GetAllEntities() const noexcept {
			return m_entities;
		}

		template<typename T, typename... Args> T&
			/**
  * @brief Quita un componente de una entidad.
  * @tparam T Tipo de componente.
  */
			AddComponent(EntityID entity, Args&&... args) {
			assert(IsAlive(entity) && "Intentando agregar un componente a una entidad no valida");
			return GetOrCreatePool<T>()->Add(entity, std::forward<Args>(args)...);

		}

		template<typename T> void
			RemoveComponent(EntityID entity) {
			if (auto* pool = GetPool<T>())
				pool->Remove(entity);
		}
			
		template<typename T>
			T& SetComponent(EntityID entity, T value) {
			assert(IsAlive(entity) && "SetComponent: entidad invalida");
			auto* pool = GetOrCreatePool<T>();
			if (pool->Contains(entity)) {
				pool->Get(entity) = std::move(value);
				return pool->Get(entity);
			}
			return pool->Add(entity, std::move(value));
		}
		
		template<typename T>
		[[nodiscard]] bool HasComponent(EntityID entity) const noexcept {
			const auto* pool = GetPoolConst<T>();
			return pool && pool->Contains(entity);
		}

		template<typename T>
		[[nodiscard]] const T& Getcomponent(EntityID entity) {
			assert(IsAlive(entity));
			auto* pool = GetPool<T>();
			assert(pool && "GetComponent: pool no existe para este tipo");
			return pool->Get(entity);
		}

		template<typename T>
		[[nodiscard]] const T& GetComponent(EntityID entity) const
		{
			assert(IsAlive(entity));
			const auto* pool = GetPoolConst<T>();
			assert(pool && "GetComponent: pool no ecxiste para este tipo");
			return pool->Get(entity);
		}

		template<typename T>
		[[nodiscard]] T* TryComponent(EntityID entity) noexcept
		{
			auto* pool = GetPool<T>();
			return pool ? pool->TryGet(entity) : nullptr;
		}
		template<typename... Components>
		[[nodiscard]] View<Components...>GetView() {
			return View<Components...>(GetOrCreatePool<Components>()...);
		}

		//sistemas
		template<typename T, typename... Args>
		T& AddSystem(Args&&... args)
		{
			static_assert(std::is_base_of_v<System, T>, "T debe derivar de ESC::System");
			auto system = std::make_unique<T>(std::forward<Args>(args)...);
			T& ref = *system;
			system->OnStart(*this);
			m_systems.push_back(std::move(system));
			return ref;
		}
		/**
  * @brief Actualiza todos los sistemas.
  * @param deltaTime Tiempo entre frames.
  */
		void UpdateSystems(float deltaTime)
		{
			for (auto& system : m_systems)
				if (system->IsEnabled())
					system->OnUpdate(*this, deltaTime);
		}

		/**
		 * @brief Elimina todos los sistemas.
		 */
		void RemoveAllSystems()
		{
			for (auto& system : m_systems)
				system->OnDestroy(*this);
			m_systems.clear();
		}

		/**
		 * @brief Limpia completamente el registro.
		 */

		//utilidades
		void
			Clear() {
			RemoveAllSystems();
			for (auto& [typeID, pool] : m_componentPools)
				pool->Clear();
			m_entities.clear();
			m_versions.clear();
			while (!m_freeList.empty())m_freeList.pop();
		}

		//Acceso a pools
		[[nodiscard]] const std::unordered_map<ComponentTypeID, std::unique_ptr<IComonentPool>>&
			GetComponentPools() const noexcept { return m_componentPools; }

		private:
			template<typename T> 
		ComponentPool<T>* GetOrCreatePool()
		{
			const ComponentTypeID typeID = GetComponentTypeID<T>();
			auto it = m_componentPools.find(typeID);
			if (it == m_componentPools.end())
			{
				auto [newIt, ok] = m_componentPools.emplace(
					typeID, std::make_unique<ComponentPool<T>>());
				return static_cast<ComponentPool<T>*>(newIt->second.get());
			}
				return static_cast<ComponentPool<T>*>(it->second.get());
			}

		template<typename T>
		ComponentPool<T>* GetPool() noexcept {
			const ComponentTypeID typeID = GetComponentTypeID<T>();
			auto it = m_componentPools.find(typeID);
			return (it != m_componentPools.end())
				? static_cast<ComponentPool<T>*>(it->second.get())
				: nullptr;
		}

		template<typename T>
		const ComponentPool<T>* GetPoolConst() const noexcept {
			const ComponentTypeID typeID = GetComponentTypeID<T>();
			auto it = m_componentPools.find(typeID);
			return (it != m_componentPools.end())
				? static_cast<const ComponentPool<T>*>(it->second.get())
				: nullptr;
		}
		

	private:
		// entidades
		std::vector<EntityID>		m_entities;
		std::vector<EntityVersion>	m_versions;
		std::queue<EntityIndex>		m_freeList;

		std::unordered_map<ComponentTypeID, std::unique_ptr<IComonentPool>> m_componentPools;
		//sistemas 
		std::vector<std::unique_ptr<System>>m_systems;
		};
	}


