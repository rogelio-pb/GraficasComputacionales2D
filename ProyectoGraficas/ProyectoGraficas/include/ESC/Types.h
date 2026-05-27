#pragma once
#include "Prerequisites.h"
//================================
// ESC :: Types.h
/// ===============================
namespace ESC {
	//Tipos primitivos
	using EntityIndex = uint32_t;//indice de entidad en el array de entidades
	using EntityVersion = uint32_t;//version de la entidad, se incrementa cada vez que se reutiliza el indice
	using EntityID = uint64_t;//ID unico de la entidad, se compone del indice y la version
	using ComponentTypeID = uint32_t;//ID unico de cada tipo de componente

	//valor centinela para @ninguna entidad
	inline constexpr EntityID NULL_ENTITY = std::numeric_limits<EntityID>::max();

	//empaquetado/desempaquetado de EntityID
	[[nodiscard]] inline EntityIndex GetEntityIndex(EntityID id) noexcept
		{
		return static_cast<EntityIndex>(id & 0xFFFF'FFFFull);
	}

	[[nodiscard]] inline EntityVersion GetEntityVersion(EntityID id) noexcept
	{
		return static_cast<EntityVersion>((id >> 32) & 0xFFFF'FFFFull);
	}
	[[nodiscard]] inline EntityID MakeEntityID(EntityIndex index, EntityVersion version) noexcept
	{
		return (static_cast<EntityID>(version) << 32) | static_cast<EntityID>(index);

	}


	//Generador de IDs de tipo de componente
	//cada tipo de T obtiene un ID unico en tiempo de ejecuci[on

	[[nodiscard]] inline ComponentTypeID NextComponentID() noexcept
	{
		static ComponentTypeID counter = 0;
		return counter++;
	}

	template<typename>
	[[nodiscard]] ComponentTypeID GetComponentTypeID() noexcept
	{
		static const ComponentTypeID id = NextComponentID();
		return id;
	}


}