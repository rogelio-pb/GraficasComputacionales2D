#pragma once
#include "ESC/Types.h"

namespace ECS
{
    struct Target
    {
        EntityID entity = NULL_ENTITY;

        Target() = default;

        explicit Target(EntityID target)
            : entity(target)
        {}
    };
}