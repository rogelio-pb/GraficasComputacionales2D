/**
 * @file SteeringSystem.h
 * @brief Sistema encargado de actualizar los comportamientos
 * de movimiento de steering de los agentes.
 */
#pragma once
#include "ESC/System.h"
#include "ESC/Registry.h"
#include "ESC/Components/Transform.h"
#include "ESC/Components/Steering.h"
#include "ESC/Components/Target.h"
#include "Velocity.h"
#include "ESC/System/SteeringBehaivors.h"
#include "ESC/Components/PathFollower.h"

namespace ECS
{
    class SteeringSystem final : public System
    {
    public:

        /**
        * @brief Actualiza todos los agentes con Steering
        * Recorre las entidades que poseen los componentes necesarios y
        * ejecuta el comportamiento seleccionado (Seek, Flee, Arrive,
        * Wander, Pursuit u Obstacle Avoidance) finalmente actualiza la
        * velocidad aplicando la fuerza calculada y limita la velocidad
        * máxima permitida.
        */
        void OnUpdate(Registry& registry, float dt) override
        {
            // --- Comportamientos que usan Target (Seek, Flee, Arrive, Pursuit, ObstacleAvoidance) ---
            registry.GetView<Transform, Velocity, Steering, Target>().Each(
                [&](EntityID,
                    Transform& transform,
                    Velocity& velocity,
                    Steering& steering,
                    Target& target)
                {
                    if (!steering.enabled)
                    {
                        velocity.velocity = { 0.f,0.f };
                        return;
                    }

                    sf::Vector2f force{ 0.f,0.f };

                    switch (steering.type)
                    {
                    case SteeringType::Seek:
                        force = SteeringBehaviors::Seek(registry, transform, velocity, steering, target.entity);
                        break;
                    case SteeringType::Flee:
                        force = SteeringBehaviors::Flee(registry, transform, velocity, steering, target.entity);
                        break;
                    case SteeringType::Arrive:
                        force = SteeringBehaviors::Arrive(registry, transform, velocity, steering, target.entity);
                        break;
                    case SteeringType::Wander:
                        force = SteeringBehaviors::Wander(transform, velocity, steering);
                        break;
                    case SteeringType::Pursuit:
                        force = SteeringBehaviors::Pursuit(registry, transform, velocity, steering, target.entity);
                        break;
                    case SteeringType::ObstacleAvoidance:
                    {
                        sf::Vector2f seekForce = SteeringBehaviors::Seek(registry, transform, velocity, steering, target.entity);
                        sf::Vector2f avoidForce = SteeringBehaviors::ObstacleAvoidance(registry, transform, velocity, steering);
                        force = seekForce + avoidForce;
                        break;
                    }
                    default:
                        break;
                    }

                    velocity.velocity += force * dt;
                    velocity.velocity = SteeringBehaviors::Limit(velocity.velocity, steering.maxSpeed);
                });

            // --- Karts siguiendo el circuito (FollowPath) ---
            registry.GetView<Transform, Velocity, Steering, PathFollower>().Each(
                [&](EntityID,
                    Transform& transform,
                    Velocity& velocity,
                    Steering& steering,
                    PathFollower& path)
                {
                    if (!steering.enabled || steering.type != SteeringType::FollowPath)
                        return;

                    sf::Vector2f force =
                        SteeringBehaviors::FollowPath(transform, velocity, steering, path);

                    velocity.velocity += force * dt;
                    velocity.velocity = SteeringBehaviors::Limit(velocity.velocity, steering.maxSpeed);
                });
        }
    };
}