#include "Prerequisites.h"
#include "Core/Window.h"
#include "Core/CShape.h"
#include "ESC/Registry.h"
#include "ESC/Components/Transform.h"
#include "ESC/Components/Render.h"
#include "ESC/System/RenderSystem.h"
#include "ESC/System/UISystem.h"
#include "Velocity.h"
#include "ESC/Components/Steering.h"
#include "ESC/Components/Target.h"
#include "ESC/System/MovementSystem.h"
#include "ESC/System/SteeringSystem.h"

Window g_window(Window(800, 600, "Labrid Engine"));
ECS::Registry registry;

void destroy()
{
    ImGui::SFML::Shutdown();
}

int main()
{
    // m_window es un puntero a sf::RenderWindow.
    if (!ImGui::SFML::Init(*g_window.m_window)) {
        return -1;
    }

    // Registrar sistemas en el ECS.
    registry.AddSystem<ECS::SteeringSystem>();
    registry.AddSystem<ECS::MovementSystem>();
    registry.AddSystem<ECS::RenderSystem>(g_window);
    registry.AddSystem<ECS::UISystem>();

    sf::Clock deltaClock;

    ECS::EntityID circle = registry.CreateEntity();
    registry.AddComponent<ECS::Transform>(circle, sf::Vector2f{ 400.f, 300.f });
    registry.AddComponent<ECS::Render>(circle, ECS::Render::Make(CIRCLE, sf::Color(100, 250, 50), "Textures/hola.png"));
    registry.AddComponent<ECS::Velocity>(circle);
    auto& steeringCircle =
    registry.AddComponent<ECS::Steering>(circle);
    steeringCircle.enabled = false;
    steeringCircle.type = ECS::SteeringType::Seek;

    registry.AddComponent<ECS::Target>(circle);


    ECS::EntityID tri = registry.CreateEntity();
    registry.AddComponent<ECS::Transform>(tri, sf::Vector2f{ 200.f, 200.f }, 45.f);
    registry.AddComponent<ECS::Render>(tri, ECS::Render::Make(TRIANGLE, sf::Color::Cyan));
    registry.AddComponent<ECS::Velocity>(tri);

    auto& steering = registry.AddComponent<ECS::Steering>(tri);
    steering.enabled = false;  
    steering.type = ECS::SteeringType::Seek;
    registry.AddComponent<ECS::Target>(
        tri);

    ECS::EntityID cam = registry.CreateEntity();
    registry.AddComponent<ECS::Transform>(cam, sf::Vector2f{ 0.f, 0.f });
    auto& camComp = registry.AddComponent<ECS::Camera>(cam);
    camComp.followTarget = circle;    // la cámara sigue al player
    camComp.followSpeed = 5.f;        // sube para que se pegue más rápido
    camComp.zoom = 1;

    while (g_window.isOpen()) {
        while (const std::optional event =
            g_window.m_window->pollEvent()) {
            // ImGui debe recibir todos los eventos de SFML.
            ImGui::SFML::ProcessEvent(*g_window.m_window, *event);

            if (event->is<sf::Event::Closed>()) {
                g_window.close();
            }
            else if (const auto* resized = event->getIf<sf::Event::Resized > ()) {
                g_window.handleResize(resized->size);
            }
    }

        const sf::Time elapsedTime = deltaClock.restart();
        const float dt = elapsedTime.asSeconds();

        // Iniciar el frame de ImGui.
        ImGui::SFML::Update(*g_window.m_window, elapsedTime);

        // Limpiar la ventana.
        g_window.clear(sf::Color::Black);

        // Renderizar los elementos de tu ECS.
        registry.UpdateSystems(dt);

        // Renderizar ImGui después de la escena.
        ImGui::SFML::Render(*g_window.m_window);

        // Presentar el frame.
        g_window.display();
    }

    destroy();

    return 0;
}