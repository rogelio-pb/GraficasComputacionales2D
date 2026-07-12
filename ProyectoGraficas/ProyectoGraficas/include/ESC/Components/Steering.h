#pragma once
#include "Prerequisites.h"

namespace ECS
{
    enum class SteeringType
    {
        Seek = 0,
        Flee,
        Arrive,
        Wander,
        Pursuit,
        ObstacleAvoidance
    };

    struct Steering
    {
        bool enabled = false;
        SteeringType type = SteeringType::Seek;

        float maxSpeed = 200.f;
        float maxForce = 50.f;

        //-------------------------
        // Wander
        //-------------------------

        float wanderRadius = 50.f;
        float wanderDistance = 80.f;
        float wanderJitter = 20.f;
        float wanderAngle = 0.f;

        //-------------------------
        // Pursuit
        //-------------------------

        float predictionTime = 0.75f;

        //-------------------------
        // Obstacle Avoidance
        //-------------------------

        float lookAhead = 80.f;
        float avoidForce = 150.f;

        Steering() = default;

        Steering(SteeringType steeringType,
            float speed = 200.f,
            float force = 50.f)
            : type(steeringType),
            maxSpeed(speed),
            maxForce(force)
        {}
    };
}