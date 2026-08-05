#pragma once

#include "Prerequisites.h"
#include "Core/Window.h"

class Circuit
{
public:

    Circuit();

    void Draw(Window& window);

    const std::vector<sf::Vector2f>& GetWaypoints() const;

private:

    sf::Texture m_texture;
    std::optional<sf::Sprite> m_sprite;

    std::vector<sf::Vector2f> m_waypoints;
};