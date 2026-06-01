#pragma once
#include "Prerequisites.h"
#include "ESC/SparseSet.h"

namespace ESC {
	class IComonentPool : public SparseSet {
	public:
		virtual ~IComonentPool() = default;
		
		//elimina la entidad del pool, si existe
		virtual void Remove(EntityID entity) = 0;

		//puntero sin tipo al comoponente (para serializar)
		virtual void* GetRaw(EntityID entity) noexcept = 0;

	};
	
	//pool tipado------------------
	template<typename T>
	class ComponentPool final : public IComonentPool
	{
	public:
		//agregar----------
		template <typename... Args>
		Add(EntityID enity, Args&&... args) {
			assert(!Contains(entity) && "La entidad ya tiene este componente");
			InsertEntity(entity);//registra la entidad en el SparseSet y obtiene su indice denso
			m_components.emplace_back(std::forward<Args>(args)...);
			return m_components.back();
		}
		//obtener-----------
		[[nodiscard]] T&
			Get(EntityID entity) noexcept {
			assert(Contains(entity) && "La entidad no tiene este componente");
			return m_component[m_sparse[GetEntityIndex(entity)]];
		}
		[[nodiscard]] T&
			Get(EntityID entity) const noexcept {
			assert(Contains(entity) && "La entidad no tiene este componente");
			return m_component[m_sparse[GetEntityIndex(entity)]];
		}
		//Devuelve nullptr si la entidad no tiene el componente
		[[nodiscard]] T*
			TryGet(EntityID entity) noexcept {
			if (!Contains(entity)) return nullptr;
			return &m_components[m_parse[GetEntityIndex(entity)]];
		}
		//eliminar-----------
		void
			Remove(EntityID entity) override {
			if (!Contains(entity)) return;
			const EntityIndex denseIdx = m_sparse[GetEntityIndex(entity)];

			//mueve el ultimo componente al hueco
			m_components[denseIdx] = std::move(m_components.back());
			m_components.pop_back();

			//sincorniza el SparseSet
			SparseSet::Remove(entity);
		}
		void
			RemoveEntity(EntityID entity) override { Remove(entity); }

		void*
			GetRaw(EntityID entity) noexcept override { return TryGet(entity); }

		//Acceso masivo util para el serializer /  sistemas-------
		[[nodiscard]] std::vector<T>&
			GetComponent() noexcept { return m_component;  }

		[[nodiscard]] const std::vector<T>&
			GetComponent() const noexcept { return m_components; }

		void
			Clear() override {
			SparseSet::Clear();
			m_components.clear();
		}


	private:
		std::vector<T> m_components; //array de componentes, paralelo a m_dense
	};
}
