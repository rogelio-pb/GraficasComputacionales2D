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
        std::vector<sf::Vector2f> waypoints;
        std::size_t currentIndex = 0;
        float arrivalRadius = 40.f;
        bool loop = true;
        float lateralOffset = 0.f;
        int lapCount = 0;
        bool finished = false;

        PathFollower() = default;

        explicit PathFollower(std::vector<sf::Vector2f> path,
            float radius = 40.f,
            bool doLoop = true)
            : waypoints(std::move(path)),
            arrivalRadius(radius),
            loop(doLoop)
        {}

        /**
         * @brief Devuelve el waypoint actual objetivo (sin offset).
         */
        const sf::Vector2f& CurrentTarget() const
        {
            return waypoints[currentIndex];
        }

        /**
         * @brief Devuelve el índice del siguiente waypoint en la ruta,
         * respetando el loop.
         */
        std::size_t NextIndex() const
        {
            std::size_t next = currentIndex + 1;

            if (next >= waypoints.size())
                next = loop ? 0 : currentIndex;

            return next;
        }

        /**
         * @brief Devuelve el punto que la entidad debe perseguir realmente:
         * el waypoint actual desplazado perpendicularmente a la dirección
         * de la ruta según lateralOffset. Si lateralOffset es 0, es igual
         * a CurrentTarget().
         */
        sf::Vector2f GetSteeringTarget() const
        {
            if (waypoints.size() < 2 || lateralOffset == 0.f)
                return CurrentTarget();

            sf::Vector2f dir = waypoints[NextIndex()] - CurrentTarget();

            float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);

            if (len == 0.f)
                return CurrentTarget();

            dir /= len;

            // Vector perpendicular a la dirección de avance.
            sf::Vector2f perp{ -dir.y, dir.x };

            return CurrentTarget() + perp * lateralOffset;
        }

        /**
         * @brief Avanza al siguiente waypoint de la ruta.
         * Si loop es true, al llegar al final vuelve al índice 0.
         */
        void Advance()
        {
            currentIndex++;

            if (currentIndex >= waypoints.size())
            {
                currentIndex = loop ? 0 : waypoints.size() - 1;

                if (loop)
                    lapCount++;
            }
        }

        /**
         * @brief Progreso total de la entidad a lo largo del circuito,
         * combinando vueltas completas + fracción del waypoint actual.
         */
        float TotalProgress() const
        {
            if (waypoints.empty())
                return 0.f;

            return static_cast<float>(lapCount) * waypoints.size() +
                static_cast<float>(currentIndex);
        }
    };
}