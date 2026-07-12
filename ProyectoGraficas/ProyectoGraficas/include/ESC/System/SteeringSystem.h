#pragma once
#include "ESC/System.h"
#include "ESC/Registry.h"
#include "ESC/Components/Transform.h"
#include "Velocity.h"
#include "ESC/Components/Steering.h"
#include "ESC/Components/Target.h"

#include <cmath>

namespace ECS
{
    class SteeringSystem final : public System
    {
    public:

        void OnUpdate(Registry& registry, float dt) override
        {
            registry.GetView<Transform, Velocity, Steering, Target>().Each(
                [this, &registry, dt](EntityID,
                    Transform& transform,
                    Velocity& velocity,
                    Steering& steering,
                    Target& target)
                {
                    if (target.entity == NULL_ENTITY)
                        return;

                    if (!registry.IsAlive(target.entity))
                        return;

                    auto* targetTransform =
                        registry.TryComponent<Transform>(target.entity);

                    if (!targetTransform)
                        return;

                    sf::Vector2f force;

                    if (!steering.enabled)
                    {
                        velocity.velocity.x = 0.f;
                        velocity.velocity.y = 0.f;
                        return;
                    }

                    switch (steering.type)
                    {
                    case SteeringType::Seek:
                        force = Seek(
                            transform.position,
                            velocity.velocity,
                            targetTransform->position,
                            steering.maxSpeed,
                            steering.maxForce);
                        break;

                    case SteeringType::Flee:
                        force = Flee(
                            transform.position,
                            velocity.velocity,
                            targetTransform->position,
                            steering.maxSpeed,
                            steering.maxForce);
                        break;

                    case SteeringType::Arrive:
                        force = Arrive(
                            transform.position,
                            velocity.velocity,
                            targetTransform->position,
                            steering.maxSpeed,
                            steering.maxForce);
                        break;

                    case SteeringType::Wander:

                        force = Wander(
                            transform,
                            velocity,
                            steering);
                        break;

                    case SteeringType::Pursuit:

                        break;

                    case SteeringType::ObstacleAvoidance:

                        break;
                    }

                    velocity.velocity += force * dt;
                    velocity.velocity = Limit(
                        velocity.velocity,
                        steering.maxSpeed);
                });
        }

    private:

        float Length(const sf::Vector2f& v)
        {
            return std::sqrt(v.x * v.x + v.y * v.y);
        }

        sf::Vector2f Normalize(const sf::Vector2f& v)
        {
            float length = Length(v);

            if (length == 0.f)
                return { 0.f,0.f };

            return v / length;
        }

        sf::Vector2f Limit(const sf::Vector2f& v, float max)
        {
            float length = Length(v);

            if (length <= max)
                return v;

            return Normalize(v) * max;
        }

        sf::Vector2f Seek(
            const sf::Vector2f& position,
            const sf::Vector2f& velocity,
            const sf::Vector2f& target,
            float maxSpeed,
            float maxForce)
        {
            sf::Vector2f desired = target - position;

            desired = Normalize(desired);

            desired *= maxSpeed;

            sf::Vector2f steering = desired - velocity;

            return Limit(steering, maxForce);
        }

        sf::Vector2f Flee(
            const sf::Vector2f& position,
            const sf::Vector2f& velocity,
            const sf::Vector2f& target,
            float maxSpeed,
            float maxForce)
        {
            sf::Vector2f desired = position - target;

            desired = Normalize(desired);

            desired *= maxSpeed;

            sf::Vector2f steering = desired - velocity;

            return Limit(steering, maxForce);
        }

        sf::Vector2f Arrive(
            const sf::Vector2f& position,
            const sf::Vector2f& velocity,
            const sf::Vector2f& target,
            float maxSpeed,
            float maxForce)
        {
            sf::Vector2f desired = target - position;

            float distance = Length(desired);

            if (distance == 0.f)
                return { 0.f,0.f };

            desired = Normalize(desired);

            if (distance < 100.f)
            {
                maxSpeed *= distance / 100.f;
            }

            desired *= maxSpeed;

            sf::Vector2f steering = desired - velocity;

            return Limit(steering, maxForce);
        }
        sf::Vector2f Wander(
            Transform& transform,
            Velocity& velocity,
            Steering& steering)
        {
            return { 0.f,0.f };
        }
    };
}