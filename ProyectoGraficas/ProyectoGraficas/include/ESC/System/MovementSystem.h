/**
 * @file MovementSystem.h
 * @brief Sistema encargado de actualizar la posición y orientación de las entidades.
 */
#pragma once
#include "ESC/System.h"
#include "ESC/Registry.h"
#include "ESC/Components/Transform.h"
#include "Velocity.h"
#include <cmath>

namespace ECS
{
    class MovementSystem final : public System
    {
    public:

        /**
         * @brief Actualiza la posición y rotación de las entidades.
         * Aplica la velocidad actual de cada entidad para calcular
         * su nueva posición, y orienta la entidad en la dirección
         * en la que se está moviendo.
         */
        void OnUpdate(Registry& registry, float dt) override
        {
            registry.GetView<Transform, Velocity>().Each(
                [dt](EntityID, Transform& transform, Velocity& velocity)
                {
                    transform.position += velocity.velocity * dt;

                    // Solo rota si hay velocidad suficiente;
                    // evita que "tiemble" de dirección cuando está casi detenido.
                    constexpr float minSpeedToRotate = 5.f;
                    float speed = std::sqrt(
                        velocity.velocity.x * velocity.velocity.x +
                        velocity.velocity.y * velocity.velocity.y);

                    if (speed > minSpeedToRotate)
                    {
                        float angleRad = std::atan2(velocity.velocity.y, velocity.velocity.x);
                        float angleDeg = angleRad * 180.f / 3.14159265f;

                        transform.rotation = angleDeg;
                    }
                });

        }
    };
}