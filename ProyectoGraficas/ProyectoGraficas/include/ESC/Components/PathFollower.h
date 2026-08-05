/**
 * @file PathFollower.h
 * @brief Componente que permite a una entidad seguir una ruta de waypoints
 * de forma independiente (cada entidad guarda su propio progreso).
 */
#pragma once
#include "Prerequisites.h"

namespace ECS
{
    /**
     * @struct PathFollower
     * @brief Almacena la ruta de waypoints y el progreso de una entidad
     * a lo largo de dicha ruta. Se usa junto con SteeringType::FollowPath.
     */
    struct PathFollower
    {
        /// Puntos que conforman la ruta (ej. el circuito).
        std::vector<sf::Vector2f> waypoints;

        /// Índice del waypoint que se está persiguiendo actualmente.
        std::size_t currentIndex = 0;

        /// Distancia a la que se considera "alcanzado" un waypoint.
        float arrivalRadius = 40.f;

        /// Si es true, al llegar al último waypoint vuelve al primero (vuelta de circuito).
        bool loop = true;

        PathFollower() = default;

        explicit PathFollower(std::vector<sf::Vector2f> path,
            float radius = 40.f,
            bool doLoop = true)
            : waypoints(std::move(path)),
            arrivalRadius(radius),
            loop(doLoop)
        {}

        /**
         * @brief Devuelve el waypoint actual objetivo.
         */
        const sf::Vector2f& CurrentTarget() const
        {
            return waypoints[currentIndex];
        }

        /**
         * @brief Avanza al siguiente waypoint de la ruta.
         * Si loop es true, al llegar al final vuelve al índice 0.
         */
        void Advance()
        {
            currentIndex++;

            if (currentIndex >= waypoints.size())
                currentIndex = loop ? 0 : waypoints.size() - 1;
        }
    };
}