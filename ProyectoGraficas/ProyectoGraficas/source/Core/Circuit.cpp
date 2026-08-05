#include "Core/Circuit.h"

Circuit::Circuit()
{
    if (!m_texture.loadFromFile("Textures/pista.jpg"))
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
        // Meta / recta inferior (hacia la derecha)
        {670.0f, 709.0f},
        {710.4f, 704.0f},
        {750.2f, 697.8f},

        // Curva derecha (subiendo)
        {787.3f, 684.7f},
        {820.8f, 663.2f},
        {849.3f, 633.7f},
        {869.4f, 599.6f},
        {880.0f, 561.6f},
        {886.0f, 521.6f},
        {883.0f, 481.2f},
        {875.8f, 441.8f},
        {860.0f, 405.9f},
        {835.6f, 373.6f},
        {804.1f, 347.1f},
        {768.8f, 329.8f},

        // Recta superior (hacia la izquierda)
        {730.0f, 321.0f},
        {690.0f, 315.0f},
        {648.9f, 312.0f},
        {606.4f, 312.0f},
        {564.0f, 312.0f},
        {521.5f, 312.0f},
        {479.1f, 312.0f},
        {436.6f, 312.0f},
        {394.2f, 312.0f},
        {352.6f, 314.0f},
        {312.2f, 319.0f},
        {272.5f, 325.5f},

        // Curva izquierda (bajando)
        {235.1f, 337.9f},
        {201.9f, 360.1f},
        {173.7f, 390.3f},
        {153.0f, 424.1f},
        {142.0f, 462.0f},
        {136.0f, 502.0f},
        {139.2f, 542.2f},
        {146.6f, 581.6f},
        {162.0f, 617.7f},
        {186.9f, 649.9f},
        {218.5f, 676.0f},
        {253.9f, 692.9f},

        // Recta inferior (regreso hacia meta)
        {293.0f, 701.0f},
        {333.3f, 706.3f},
        {374.1f, 710.0f},
        {416.2f, 711.0f},
        {458.6f, 711.0f},
        {501.1f, 711.0f},
        {543.5f, 711.0f},
        {585.9f, 711.0f},
        {628.4f, 711.0f}
    };
}

const std::vector<sf::Vector2f>& Circuit::GetWaypoints() const
{
    return m_waypoints;
}

void Circuit::Draw(Window& window)
{
    

    for (const auto& waypoint : m_waypoints)
    {
        sf::CircleShape shape(5.f);
        shape.setPosition(waypoint);
        window.m_window->draw(shape);
    }
    window.m_window->draw(*m_sprite);
    
}