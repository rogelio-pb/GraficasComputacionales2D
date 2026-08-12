#include "Core/Circuit.h"

Circuit::Circuit()
{
    if (!m_texture.loadFromFile("Textures/pista1.png"))
    {
        std::cout << "No se pudo cargar la textura\n";
    }

    m_sprite.emplace(m_texture);

    // Escalar para que ocupe la ventana (800x600)
    sf::Vector2u size = m_texture.getSize();

    m_sprite->setScale({ 1.f,1.f });
    

    // Waypoints (ejemplo)
	m_waypoints.emplace_back(100.f, 100.f);
    m_waypoints =
    {
        // Recta de meta (hacia la derecha)
        {342.0f, 899.0f},
        {421.0f, 899.0f},
        {499.9f, 899.0f},
        {578.9f, 899.0f},
        {657.8f, 899.0f},
        {736.8f, 899.0f},
        {815.7f, 899.0f},
        {894.7f, 899.0f},
        {973.6f, 899.0f},
        {1052.6f, 899.0f},
        {1131.5f, 899.0f},
        {1210.1f, 898.0f},

        // Curva derecha hacia el primer serpenteo
        {1281.1f, 878.9f},
        {1333.0f, 822.0f},
        {1384.6f, 764.4f},
        {1436.9f, 707.1f},
        {1459.0f, 637.3f},
        {1440.8f, 566.8f},
        {1372.2f, 545.0f},
        {1303.4f, 569.6f},
        {1238.2f, 602.8f},
        {1173.1f, 636.0f},
        {1102.4f, 654.0f},
        {1035.0f, 626.0f},
        {993.7f, 567.7f},
        {1008.8f, 497.2f},
        {1067.1f, 448.9f},
        {1142.0f, 447.0f},
        {1216.6f, 456.4f},

        // Curva rápida derecha (esquina superior derecha)
        {1283.3f, 427.0f},
        {1340.0f, 375.4f},
        {1346.0f, 303.1f},
        {1299.4f, 243.4f},
        {1243.6f, 187.6f},
        {1187.5f, 132.5f},
        {1121.2f, 102.0f},
        {1050.6f, 122.4f},
        {985.2f, 155.0f},
        {919.9f, 188.0f},
        {854.2f, 220.0f},
        {788.9f, 253.1f},
        {723.7f, 286.3f},

        // Chicane central (la "S" del medio)
        {669.0f, 334.3f},
        {647.6f, 404.4f},
        {668.7f, 472.7f},
        {726.4f, 518.4f},
        {728.0f, 591.7f},
        {670.2f, 631.0f},
        {598.2f, 618.2f},
        {533.6f, 583.6f},
        {498.0f, 521.1f},
        {487.0f, 446.7f},
        {479.0f, 371.0f},
        {467.7f, 296.7f},

        // Curva superior izquierda
        {423.4f, 241.4f},
        {355.6f, 214.6f},
        {287.6f, 188.0f},
        {215.3f, 176.0f},
        {157.0f, 216.0f},
        {121.0f, 280.0f},
        {124.0f, 352.8f},
        {174.7f, 410.7f},

        // Bajada por la izquierda de regreso a meta
        {229.1f, 467.1f},
        {275.0f, 527.1f},
        {268.0f, 599.0f},
        {210.9f, 649.0f},
        {160.0f, 702.8f},
        {161.0f, 776.4f},
        {210.7f, 834.7f},
        {268.6f, 885.6f}
    };
}

const std::vector<sf::Vector2f>& Circuit::GetWaypoints() const
{
    return m_waypoints;
}

void Circuit::Draw(Window& window)
{
    window.m_window->draw(*m_sprite);

    // Línea que conecta todos los waypoints (+1 para cerrar el loop).
    sf::VertexArray line(sf::PrimitiveType::LineStrip, m_waypoints.size() + 1);

    for (std::size_t i = 0; i < m_waypoints.size(); ++i)
    {
        line[i].position = m_waypoints[i];
        line[i].color = sf::Color::Red;
    }

    // Último vértice = el primer waypoint, para cerrar el circuito visualmente.
    line[m_waypoints.size()].position = m_waypoints[0];
    line[m_waypoints.size()].color = sf::Color::Red;

    window.m_window->draw(line);

    // Puntos de cada waypoint (ya lo tenías).
    for (const auto& waypoint : m_waypoints)
    {
        sf::CircleShape shape(5.f);
        shape.setOrigin({ 5.f, 5.f });   
        shape.setPosition(waypoint);
        shape.setFillColor(sf::Color::Blue);
        window.m_window->draw(shape);
    }
}