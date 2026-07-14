/**
 * @file Steering.h
 * @brief Define los tipos de comportamiento y la configuración utilizada
 * por el sistema de Steering.
 */
#pragma once
#include "Prerequisites.h"

namespace ECS
{
    enum class SteeringType
    {
        /// El agente se dirige directamente hacia el objetivo.
        Seek = 0,
        Flee,
        Arrive,
        Wander,
        Pursuit,
        ObstacleAvoidance
    };

     /**
     * @struct Steering
     * @brief Almacena la configuración del comportamiento de movimiento.
     * Este componente guarda toda la información necesaria para que el
     * sistema de Steering pueda calcular el movimiento de una entidad.
     * Dependiendo del comportamiento seleccionado, algunos parámetros
     * serán utilizados y otros permanecerán sin cambios.
     */
    struct Steering
    {
        bool enabled = false;
        /**
  * @brief Comportamiento que utilizará la entidad.
  */
        SteeringType type = SteeringType::Seek;

		//Velocidad máxima y fuerza máxima que puede aplicar el agente
        float maxSpeed = 200.f;
        float maxForce = 50.f;

 
		//Radio de búsqueda y distancia de predicción para el comportamiento de Arrive
        float wanderRadius = 50.f;
        float wanderDistance = 80.f;
        float wanderJitter = 20.f;
        float wanderAngle = 0.f;

    
		// Tiempo de predicción para el comportamiento de Pursuit
        float predictionTime = 0.75f;

        
		// Parámetros para el comportamiento de Obstacle Avoidance
        float lookAhead = 80.f;
        float avoidForce = 150.f;
        float avoidRadius = 80.f;

        Steering() = default;

        /**
         * @brief Crea el componente con una configuración inicial.
         * @param steeringType Tipo de comportamiento que utilizará la entidad.
         * @param speed Velocidad máxima que puede alcanzar la entidad.
         * @param force Fuerza máxima que puede aplicar la entidad.
         */
        Steering(SteeringType steeringType,
            float speed = 200.f,
            float force = 50.f)
            : type(steeringType),
            maxSpeed(speed),
            maxForce(force)
        {}
    };
}