/**
 * @file Collider.h
 * @brief Componente simple de colisión circular, usado para detectar
 * overlap entre entidades  y resolverlo empujandolas aparte.
 */
#pragma once
#include "Prerequisites.h"

namespace ECS
{
    /**
     * @struct Collider
     * @brief Define el radio de colisión circular de una entidad.
     * No implementa física real, solo detección de overlap por distancia.
     */
    struct Collider
    {
        float radius = 12.f;

        Collider() = default;
        explicit Collider(float r) : radius(r) {}
    };
}