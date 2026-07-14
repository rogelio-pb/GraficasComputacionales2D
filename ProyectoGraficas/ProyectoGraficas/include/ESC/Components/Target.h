/**
 * @file Target.h
 * @brief Componente que almacena la entidad objetivo de un agente.
 */
#pragma once
#include "ESC/Types.h"

namespace ECS
{
    /**
     * @struct Target
     * @brief Guarda la entidad que será utilizada como objetivo
     * Este componente permite que un agente conozca qué entidad debe
     * seguir, perseguir o de la cual debe alejarse, dependiendo del
     * comportamiento de Steering que esté utilizando
     */
    struct Target
    {
        EntityID entity = NULL_ENTITY;

        Target() = default;

         /**
         * @brief Crea el componente asignando una entidad objetivo
         * @param target Identificador de la entidad que será el objetivo
         */

        explicit Target(EntityID target)
            : entity(target)
        {}
    };
}