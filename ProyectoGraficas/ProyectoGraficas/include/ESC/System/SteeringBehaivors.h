/**
 * @file SteeringBehaviors.h
 * @brief Contiene la implementación de los diferentes comportamientos
 * de movimiento (Steering Behaviors) utilizados por los agentes
 */
#pragma once

#include "Prerequisites.h"
#include "ESC/Registry.h"
#include "ESC/Components/Transform.h"
#include "ESC/Components/Steering.h"
#include "Velocity.h"
namespace ECS::SteeringBehaviors
{

    //utilidades
    inline float Length(const sf::Vector2f& v)
    {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }

    /**
    * @brief Limita la magnitud de un vector a un valor máximo
    * @param v Vector a limitar
    * @param max Valor máximo permitido
    * @return Vector limitado
    */
    inline sf::Vector2f Normalize(const sf::Vector2f& v)
    {
        float length = Length(v);

        if (length == 0.f)
            return { 0.f,0.f };

        return v / length;
    }

    inline sf::Vector2f Limit(const sf::Vector2f& v, float max)
    {
        float length = Length(v);

        if (length <= max)
            return v;

        return Normalize(v) * max;
    }

    /**
     * @brief Hace que el agente se dirija directamente hacia un objetivo
     * Obtiene la posición del objetivo y calcula la fuerza necesaria
     * para acercarse respetando la velocidad y fuerza máximas
     */
    inline sf::Vector2f Seek(
        Registry& registry,
        Transform& transform,
        Velocity& velocity,
        Steering& steering,
        EntityID targetEntity)
        /**
		* @brief Calcula la fuerza de búsqueda hacia un objetivo
        */
    {
        if (targetEntity == NULL_ENTITY)
            return { 0.f,0.f };

        auto* targetTransform =
            registry.TryComponent<Transform>(targetEntity);

        if (!targetTransform)
            return { 0.f,0.f };

        sf::Vector2f desired =
            targetTransform->position - transform.position;

        desired = Normalize(desired);
        desired *= steering.maxSpeed;

        sf::Vector2f force =
            desired - velocity.velocity;

        return Limit(force, steering.maxForce);
    }


    /**
    * @brief Hace que el agente se aleje del objetivo
    * Funciona de manera similar a Seek, pero invirtiendo la dirección
    * para escapar del objetivo
    */
    inline sf::Vector2f Flee(
        Registry& registry,
        Transform& transform,
        Velocity& velocity,
        Steering& steering,
        EntityID targetEntity)
        /**
		* @brief Calcula la fuerza de escape del objetivo
        */
    {
        if (targetEntity == NULL_ENTITY)
            return { 0.f,0.f };

        auto* targetTransform =
            registry.TryComponent<Transform>(targetEntity);

        if (!targetTransform)
            return { 0.f,0.f };

        sf::Vector2f desired =
            transform.position - targetTransform->position;

        desired = Normalize(desired);
        desired *= steering.maxSpeed;

        sf::Vector2f force =
            desired - velocity.velocity;

        return Limit(force, steering.maxForce);
    }

	/**
	* @brief Hace que el agente se acerque al objetivo y desacelere
    * Cuando el agente está lejos avanza a máxima velocidad
    * Al acercarse comienza a disminuir la velocidad para evitar
    * detenerse de forma brusca
    */
    inline sf::Vector2f Arrive(
        Registry& registry,
        Transform& transform,
        Velocity& velocity,
        Steering& steering,
        EntityID targetEntity)
        /**
		* @brief Calcula la fuerza de llegada al objetivo
        */
    {
        if (targetEntity == NULL_ENTITY)
            return { 0.f,0.f };

        auto* targetTransform =
            registry.TryComponent<Transform>(targetEntity);

        if (!targetTransform)
            return { 0.f,0.f };

        sf::Vector2f desired =
            targetTransform->position -
            transform.position;

        float distance =
            Length(desired);

        if (distance == 0.f)
            return { 0.f,0.f };

        desired = Normalize(desired);

        float speed = steering.maxSpeed;

        if (distance < 100.f)
            speed *= distance / 100.f;

        desired *= speed;

        sf::Vector2f force =
            desired - velocity.velocity;

        return Limit(force, steering.maxForce);
    }

    /**
 * @brief Genera un movimiento aleatorio
 * El agente cambia ligeramente su dirección en cada actualización,
 * simulando un recorrido libre por el escenario
 * Hace uso de un círculo imaginario delante del agente para determinar su nueva direcció
 */
    inline sf::Vector2f Wander(
        Transform& transform,
        Velocity& velocity,
        Steering& steering)

        /**
		* @brief Calcula la fuerza de wander
        */
    {
        float random =
            ((float)std::rand() / RAND_MAX) * 2.f - 1.f;

        steering.wanderAngle +=
            random * steering.wanderJitter;

        sf::Vector2f forward;

        if (Length(velocity.velocity) < 0.01f)
            forward = { 1.f,0.f };
        else
            forward = Normalize(velocity.velocity);

        sf::Vector2f circleCenter =
            transform.position +
            forward * steering.wanderDistance;

        sf::Vector2f displacement;

        displacement.x =
            std::cos(steering.wanderAngle);

        displacement.y =
            std::sin(steering.wanderAngle);

        displacement *= steering.wanderRadius;

        sf::Vector2f target =
            circleCenter + displacement;

        sf::Vector2f desired =
            target - transform.position;

        desired = Normalize(desired);
        desired *= steering.maxSpeed;

        sf::Vector2f force =
            desired - velocity.velocity;

        return Limit(force, steering.maxForce);
    }

    /**
    * @brief Perseguir a un objetivo
    * El agente sigue a un objetivo prediciendo su posición futura
    * Si el objetivo está en movimiento se estima dónde estará
    * después de un tiempo para interceptarlo de manera más eficiente
    */
    inline sf::Vector2f Pursuit(
        Registry& registry,
        Transform& transform,
        Velocity& velocity,
        Steering& steering,
        EntityID targetEntity)

        /**
		* @brief Calcula la fuerza de persecución del objetivo
        */
    {
        auto* targetTransform =
            registry.TryComponent<Transform>(targetEntity);

        auto* targetVelocity =
            registry.TryComponent<Velocity>(targetEntity);

        if (!targetTransform)
            return { 0.f,0.f };

        sf::Vector2f futurePosition =
            targetTransform->position;

        if (targetVelocity)
        {
            futurePosition +=
                targetVelocity->velocity *
                steering.predictionTime;
        }
        /**
        * @brief Calcular la fuerza de persecución
        */
        sf::Vector2f desired =
            futurePosition -
            transform.position;

        desired = Normalize(desired);
        desired *= steering.maxSpeed;

        sf::Vector2f force =
            desired -
            velocity.velocity;

        return Limit(force, steering.maxForce);
    }

    /**
    * @brief Evita obstáculos cercanos
    * Busca obstáculos frente al agente y genera una fuerza de
    * desviación para rodearlos y continuar el recorrido
	* Obtiene la posición de los obstáculos y calcula la fuerza necesaria
    */

    inline sf::Vector2f ObstacleAvoidance(
        Registry& registry,
        Transform& transform,
        Velocity& velocity,
        Steering& steering)
    {
        /**
        * @brief Dirección de movimiento
        */
        sf::Vector2f forward;

        if (Length(velocity.velocity) < 0.01f)
            return { 0.f,0.f };

        forward = Normalize(velocity.velocity);

        /**
        * @brief Punto adelante del agente
        */
        sf::Vector2f ahead =
            transform.position +
            forward * steering.lookAhead;

        /**
        * @brief Buscar obstáculo más cercano
        */
        float closestDistance = 999999.f;

        Transform* closestObstacle = nullptr;

        registry.GetView<Transform>().Each(
            [&](EntityID id, Transform& other)
            {
                if (&other == &transform)
                    return;

                float distance =
                    Length(other.position - ahead);

                if (distance < steering.avoidRadius &&
                    distance < closestDistance)
                {
                    closestDistance = distance;
                    closestObstacle = &other;
                }
            });

        /*
		* @brief Si no hay obstáculos cercanos, no se aplica fuerza de evasión
        */
        if (!closestObstacle)
            return { 0.f,0.f };

        /**
		* @brief Calcular fuerza de evasión
        */
        sf::Vector2f avoid =
            ahead -
            closestObstacle->position;

        avoid = Normalize(avoid);

        avoid *= steering.avoidForce * 5.f;

        return avoid;
    }
}