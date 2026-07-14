/**
 * @file MovementSystem.h
 * @brief Sistema encargado de actualizar la posición de las entidades.
 */
#pragma once
#include "ESC/System.h"
#include "ESC/Registry.h"
#include "ESC/Components/Transform.h"
#include "Velocity.h"

namespace ECS
{
    class MovementSystem final : public System
    {
    public:

    /**
     * @brief Actualiza la posición de las entidades.
     * Aplica la velocidad actual de cada entidad para calcular
     * su nueva posición dentro del escenario.
     */
        void OnUpdate(Registry& registry, float dt) override
        {
            registry.GetView<Transform, Velocity>().Each(
                [dt](EntityID, Transform& transform, Velocity& velocity)
                {
                    transform.position += velocity.velocity * dt;

                });
        }
    };
}