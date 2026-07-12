#pragma once
#include "Prerequisites.h"

namespace ECS
{
    struct Velocity
    {
        sf::Vector2f velocity{ 0.f, 0.f };

        Velocity() = default;

        explicit Velocity(sf::Vector2f vel)
            : velocity(vel)
        {}
    };
}