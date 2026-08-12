/**
 * @file CollisionSystem.h
 * @brief Sistema encargado de detectar y resolver colisiones simples
 * (overlap) entre entidades con Collider, separándolas por penetración.
 */
#pragma once
#include "ESC/System.h"
#include "ESC/Registry.h"
#include "ESC/Components/Transform.h"
#include "ESC/Components/Collider.h"
#include "Velocity.h"
#include <cmath>

namespace ECS
{
    class CollisionSystem final : public System
    {
    public:

        /**
         * @brief Revisa cada par de entidades con Collider y, si están
         * superpuestas (distancia < radio1 + radio2), las separa
         * empujándolas en direcciones opuestas según la penetración.
         * No aplica física real solo correccion de posicion.
         */
        void OnUpdate(Registry& registry, float dt) override
        {
            registry.GetView<Transform, Collider>().Each(
                [&](EntityID idA, Transform& transformA, Collider& colliderA)
                {
                    registry.GetView<Transform, Collider>().Each(
                        [&](EntityID idB, Transform& transformB, Collider& colliderB)
                        {
                            // Procesa cada par una sola vez y evita comparar consigo misma.
                            if (idB <= idA)
                                return;

                            sf::Vector2f diff =
                                transformB.position - transformA.position;

                            float distance =
                                std::sqrt(diff.x * diff.x + diff.y * diff.y);

                            float minDistance =
                                colliderA.radius + colliderB.radius;

                            if (distance >= minDistance || distance == 0.f)
                                return;

                            float penetration = minDistance - distance;

                            sf::Vector2f normal = diff / distance;

                            // Separa a cada una la mitad de la penetración,
                            // en direcciones opuestas.
                            transformA.position -= normal * (penetration * 0.5f);
                            transformB.position += normal * (penetration * 0.5f);
                        });
                });
        }
    };
}