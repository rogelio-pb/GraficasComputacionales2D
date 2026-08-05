/**
 * @file main.cpp
 * @brief Punto de entrada de la aplicación.
 * Inicializa la ventana, el sistema ECS, ImGui y crea las entidades
 * necesarias para probar los diferentes comportamientos de Steering
 */
#include "Prerequisites.h"
#include "Core/Window.h"
#include "ESC/System/CameraSystem.h"
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
#include "Core/Circuit.h"
#include "ESC/Components/PathFollower.h"

Window g_window(Window(800, 600, "Labrid Engine"));
ECS::Registry registry;
Circuit circuit;

void destroy()
{
    ImGui::SFML::Shutdown();
}

/**
 * @brief Función principal del programa
 * Inicializa la ventana, registra los sistemas del ECS,
 * crea las entidades de prueba y ejecuta el ciclo principal
 * del motor hasta que el usuario cierre la aplicación
 */
int main()
{
    // m_window es un puntero a sf::RenderWindow.
    if (!ImGui::SFML::Init(*g_window.m_window)) {
        return -1;
    }

    // Registrar sistemas en el ECS.
    registry.AddSystem<ECS::SteeringSystem>();
    registry.AddSystem<ECS::MovementSystem>();
    registry.AddSystem<ECS::CameraSystem>(g_window);
    registry.AddSystem<ECS::RenderSystem>(g_window);
    registry.AddSystem<ECS::UISystem>(g_window);

    sf::Clock deltaClock;

    // --- Karts que siguen el circuito (FollowPath) ---
    const auto& trackWaypoints = circuit.GetWaypoints();

    const int numKarts = 4;

    for (int i = 0; i < numKarts; ++i)
    {
        std::size_t startIndex = (i * trackWaypoints.size()) / numKarts;

        ECS::EntityID kart = registry.CreateEntity();

        registry.AddComponent<ECS::Transform>(kart, trackWaypoints[startIndex]);
        registry.AddComponent<ECS::Render>(
            kart,
            ECS::Render::Make(CIRCLE, sf::Color(100 + i * 30, 250 - i * 20, 50), "Textures/images.jpg"));
        registry.AddComponent<ECS::Velocity>(kart);

        auto& steeringKart = registry.AddComponent<ECS::Steering>(kart);
        steeringKart.enabled = true;
        steeringKart.type = ECS::SteeringType::FollowPath;
        steeringKart.maxSpeed = 100.f + i * 15.f;   // antes: 150 + i*20
        steeringKart.maxForce = 300.f;              // antes: 60.f

        auto& path = registry.AddComponent<ECS::PathFollower>(kart);
        path.waypoints = trackWaypoints;
        path.currentIndex = startIndex;
        path.arrivalRadius = 40.f;
        path.loop = true;
    }

    // --- Entidades de prueba (se dejan igual, sin FollowPath) ---
    ECS::EntityID circle1 = registry.CreateEntity();
    registry.AddComponent<ECS::Transform>(circle1, sf::Vector2f{ 200.f, 10.f });
    registry.AddComponent<ECS::Render>(circle1, ECS::Render::Make(CIRCLE, sf::Color(255, 255, 0)));
    registry.AddComponent<ECS::Velocity>(circle1);
    auto& steeringCircle1 =
        registry.AddComponent<ECS::Steering>(circle1);
    steeringCircle1.enabled = false;
    steeringCircle1.type = ECS::SteeringType::Seek;

    registry.AddComponent<ECS::Target>(circle1);


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
    camComp.followTarget = ECS::NULL_ENTITY;
    camComp.followSpeed = 5.f;        // sube para que se pegue más rápido
    camComp.zoom = 1;

    // Inicializar la semilla para la generación de números aleatorios
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Bucle principal del motor
    while (g_window.isOpen()) {
        while (const std::optional event =
            g_window.m_window->pollEvent()) {
            // ImGui debe recibir todos los eventos de SFML.
            ImGui::SFML::ProcessEvent(*g_window.m_window, *event);

            //Se manejan los eventos de cierre y redimensionamiento de la ventana
            if (event->is<sf::Event::Closed>()) {
                g_window.close();
            }
            else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                g_window.handleResize(resized->size);
            }
        }

        const sf::Time elapsedTime = deltaClock.restart();
        const float dt = elapsedTime.asSeconds();

        // Iniciar el frame de ImGui.
        ImGui::SFML::Update(*g_window.m_window, elapsedTime);

        // Limpiar la ventana.
        g_window.clear(sf::Color::Black);

        // Dibujar el circuito.
        circuit.Draw(g_window);

        // Actualizar los sistemas del ECS (incluye el avance de cada kart por su PathFollower).
        registry.UpdateSystems(dt);

        // Renderizar ImGui después de la escena.
        ImGui::SFML::Render(*g_window.m_window);

        // Aplicar cambios pendientes del MSAA.
        //g_window.applyPendingMSAA();

        // Presentar el frame.
        g_window.display();
    }

    destroy();

    return 0;
}