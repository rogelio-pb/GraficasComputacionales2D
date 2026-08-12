#pragma once
#include "Prerequisites.h"
#include "Core/Window.h"

/**
 * @class Circuit
 * @brief Representa la pista del circuito y su ruta de waypoints.
 *
 * Carga la textura de la pista, la dibuja en pantalla y expone una lista
 * de waypoints puntos de la ruta que las entidades con PathFollower
 */
class Circuit
{
public:
    Circuit();

    void Draw(Window& window);
    /**
     * @brief Devuelve la lista de waypoints del circuito.
     * @return Referencia constante al vector de posiciones que conforman
     * la ruta del circuito, en el orden en que deben recorrerse.
     */
    const std::vector<sf::Vector2f>& GetWaypoints() const;

private:

    /// Textura de la imagen de la pista
    sf::Texture m_texture;

    /// Sprite que representa visualmente la pista, construido sobre m_texture
    std::optional<sf::Sprite> m_sprite;

    /// Puntos ordenados que conforman la ruta del circuito
    std::vector<sf::Vector2f> m_waypoints;
};