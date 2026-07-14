/**
 * @file Velocity.h
 * @brief Componente que almacena la velocidad de una entidad.
 */
#pragma once
#include "Prerequisites.h"

namespace ECS
{
    /**
    * @brief Velocidad actual de la entidad.
    * El vector indica tanto la dirección como la rapidez del movimiento.
    */
    struct Velocity
    {
        sf::Vector2f velocity{ 0.f, 0.f };

        Velocity() = default;

        /**
         * @brief Crea el componente con una velocidad inicial.
         * @param vel Velocidad que tendrá la entidad al crearse.
         */
        explicit Velocity(sf::Vector2f vel)
            : velocity(vel)
        {}
    };
}