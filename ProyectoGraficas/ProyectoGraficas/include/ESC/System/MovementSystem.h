#pragma once

#include "ESC/System.h"
#include "ESC/Registry.h"
#include "ESC/Components/Transform.h"
#include "Velocity.h"

namespace ECS
{
    class MovementSystem final : public System
    {
    public:

        void OnUpdate(Registry& registry, float dt) override
        {
            registry.GetView<Transform, Velocity>().Each(
                [dt](EntityID, Transform& transform, Velocity& velocity)
                {
                    transform.position += velocity.velocity * dt;

                });
        }
    };
}