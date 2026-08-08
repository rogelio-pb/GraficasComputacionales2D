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
#include "ESC/Components/Collider.h"
#include "ESC/System/ColliderSystem.h"

Window g_window(Window(800, 600, "Labrid Engine"));
ECS::Registry registry;
Circuit circuit;

void destroy()
{
    ImGui::SFML::Shutdown();
}

int main()
{
    if (!ImGui::SFML::Init(*g_window.m_window)) {
        return -1;
    }

    registry.AddSystem<ECS::SteeringSystem>();
    registry.AddSystem<ECS::MovementSystem>();
    registry.AddSystem<ECS::CollisionSystem>();
    registry.AddSystem<ECS::CameraSystem>(g_window);
    registry.AddSystem<ECS::RenderSystem>(g_window);
    registry.AddSystem<ECS::UISystem>(g_window);

    sf::Clock deltaClock;

    // --- Karts que siguen el circuito (FollowPath) ---
    const auto& trackWaypoints = circuit.GetWaypoints();
    const int numKarts = 4;
    const int totalLaps = 3;

    // Guardamos referencia de cada kart (entidad + color + nombre) para
    // poder armar el podio sin tener que ir a buscarlo componente por componente.
    struct KartInfo
    {
        ECS::EntityID entity;
        sf::Color color;
        std::string name;
    };
    std::vector<KartInfo> karts;

    // Colores de "equipo" para diferenciar los 4 karts, mismo archivo de textura.
    std::vector<sf::Color> kartColors = {
        sf::Color(255, 80,  80),   // rojo
        sf::Color(80,  140, 255),  // azul
        sf::Color(255, 220, 60),   // amarillo
        sf::Color(80,  220, 120),  // verde
    };

    for (int i = 0; i < numKarts; ++i)
    {
        std::size_t startIndex = (i * trackWaypoints.size()) / numKarts;

        ECS::EntityID kart = registry.CreateEntity();

        registry.AddComponent<ECS::Transform>(kart, trackWaypoints[startIndex]);

        sf::Color kartColor = kartColors[i % kartColors.size()];

        auto& kartRender = registry.AddComponent<ECS::Render>(
            kart,
            ECS::Render::Make(RECTANGLE, kartColor, "Textures/Car.png"));

        if (auto* rect = dynamic_cast<sf::RectangleShape*>(kartRender.shape.get()))
        {
            rect->setSize({ 80.f, 40.f });
            rect->setOrigin({ 40.f, 20.f });
        }

        registry.AddComponent<ECS::Velocity>(kart);

        auto& steeringKart = registry.AddComponent<ECS::Steering>(kart);
        steeringKart.enabled = true;
        steeringKart.type = ECS::SteeringType::FollowPath;
        steeringKart.maxSpeed = 100.f + i * 15.f;
        steeringKart.maxForce = 300.f;

        auto& path = registry.AddComponent<ECS::PathFollower>(kart);
        path.waypoints = trackWaypoints;
        path.currentIndex = startIndex;
        path.arrivalRadius = 40.f;
        path.loop = true;
        path.lateralOffset = (i - (numKarts - 1) / 2.f) * 15.f;

        registry.AddComponent<ECS::Collider>(kart, 12.f);

        karts.push_back({ kart, kartColor, "Kart " + std::to_string(i + 1) });
    }

    // --- Entidades de prueba (sin cambios) ---
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
    registry.AddComponent<ECS::Target>(tri);

    ECS::EntityID cam = registry.CreateEntity();
    registry.AddComponent<ECS::Transform>(cam, sf::Vector2f{ 0.f, 0.f });
    auto& camComp = registry.AddComponent<ECS::Camera>(cam);
    camComp.followTarget = ECS::NULL_ENTITY;
    camComp.followSpeed = 5.f;
    camComp.zoom = 1;

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Orden de llegada: se va llenando conforme cada kart termina totalLaps.
    std::vector<ECS::EntityID> finishOrder;

    while (g_window.isOpen()) {
        while (const std::optional event =
            g_window.m_window->pollEvent()) {
            ImGui::SFML::ProcessEvent(*g_window.m_window, *event);

            if (event->is<sf::Event::Closed>()) {
                g_window.close();
            }
            else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
                g_window.handleResize(resized->size);
            }
        }

        const sf::Time elapsedTime = deltaClock.restart();
        const float dt = elapsedTime.asSeconds();

        ImGui::SFML::Update(*g_window.m_window, elapsedTime);

        g_window.clear(sf::Color::Black);
        circuit.Draw(g_window);

        registry.UpdateSystems(dt);

        // --- Detectar llegadas y llenar finishOrder ---
        for (auto& k : karts)
        {
            auto& path = registry.GetComponent<ECS::PathFollower>(k.entity);

            if (!path.finished && path.lapCount >= totalLaps)
            {
                path.finished = true;
                finishOrder.push_back(k.entity);

                // Detenemos al kart que ya terminó.
                registry.GetComponent<ECS::Steering>(k.entity).enabled = false;
                registry.GetComponent<ECS::Velocity>(k.entity).velocity = { 0.f, 0.f };
            }
        }

        // --- HUD: vuelta actual de cada kart (mientras no ha terminado la carrera) ---
        ImGui::Begin("Carrera");
        for (auto& k : karts)
        {
            auto& path = registry.GetComponent<ECS::PathFollower>(k.entity);
            ImVec4 col(k.color.r / 255.f, k.color.g / 255.f, k.color.b / 255.f, 1.f);

            if (path.finished)
                ImGui::TextColored(col, "%s - META", k.name.c_str());
            else
                ImGui::TextColored(col, "%s - Vuelta %d/%d",
                    k.name.c_str(), path.lapCount + 1, totalLaps);
        }
        ImGui::End();

        // --- Podio: se muestra en cuanto haya al menos 1 kart en meta ---
        if (!finishOrder.empty())
        {
            ImGui::Begin("Podio");

            const char* medals[3] = { "1er lugar", "2do lugar", "3er lugar" };

            for (std::size_t pos = 0; pos < finishOrder.size() && pos < 3; ++pos)
            {
                ECS::EntityID id = finishOrder[pos];

                // Buscamos el KartInfo correspondiente para nombre y color.
                auto it = std::find_if(karts.begin(), karts.end(),
                    [id](const KartInfo& k) { return k.entity == id; });

                if (it != karts.end())
                {
                    ImVec4 col(it->color.r / 255.f, it->color.g / 255.f, it->color.b / 255.f, 1.f);
                    ImGui::TextColored(col, "%s: %s", medals[pos], it->name.c_str());
                }
            }

            if (finishOrder.size() == karts.size())
                ImGui::Text("Carrera terminada!");

            ImGui::End();
        }

        ImGui::SFML::Render(*g_window.m_window);
        g_window.display();
    }

    destroy();

    return 0;
}