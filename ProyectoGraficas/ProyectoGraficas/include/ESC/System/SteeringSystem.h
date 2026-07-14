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
            registry.GetView<Transform, Velocity, Steering, Target>().Each(
                [&](EntityID,
                    Transform& transform,
                    Velocity& velocity,
                    Steering& steering,
                    Target& target)
                {
                    /// Fuerza que se aplicará durante esta actualización.
                    sf::Vector2f force{ 0.f,0.f };

                    // Si el Steering está desactivado el agente se detiene.
                    if (!steering.enabled)
                    {
                        velocity.velocity = { 0.f,0.f };
                        return;
                    }

                    // Ejecuta el comportamiento seleccionado.
                    switch (steering.type)
                    {
                    case SteeringType::Seek:
                        force = SteeringBehaviors::Seek(
                            registry,
                            transform,
                            velocity,
                            steering,
                            target.entity);
                        break;

                    case SteeringType::Flee:
                        force = SteeringBehaviors::Flee(
                            registry,
                            transform,
                            velocity,
                            steering,
                            target.entity);
                        break;

                    case SteeringType::Arrive:
                        force = SteeringBehaviors::Arrive(
                            registry,
                            transform,
                            velocity,
                            steering,
                            target.entity);
                        break;

                    case SteeringType::Wander:
                        force = SteeringBehaviors::Wander(
                            transform,
                            velocity,
                            steering);
                        break;

                    case SteeringType::Pursuit:
                        force = SteeringBehaviors::Pursuit(
                            registry,
                            transform,
                            velocity,
                            steering,
                            target.entity);
                        break;

                    case SteeringType::ObstacleAvoidance:
                        sf::Vector2f seekForce =
                            SteeringBehaviors::Seek(
                                registry,
                                transform,
                                velocity,
                                steering,
                                target.entity);

                        sf::Vector2f avoidForce =
                            SteeringBehaviors::ObstacleAvoidance(
                                registry,
                                transform,
                                velocity,
                                steering);

                        // Combina Seek con Obstacle Avoidance
                       // para que el agente siga al objetivo
                       // mientras evita obstáculos
                        force = seekForce + avoidForce;

                        break;
                    }

                    velocity.velocity += force * dt;

                    // Evita que el agente supere su velocidad máxima.
                    velocity.velocity =
                        SteeringBehaviors::Limit(
                            velocity.velocity,
                            steering.maxSpeed);
                });
        }
    };
}