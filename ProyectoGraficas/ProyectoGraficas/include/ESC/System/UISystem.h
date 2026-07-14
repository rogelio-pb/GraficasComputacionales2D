#pragma once
#include "Prerequisites.h"
#include "ESC/System.h"
#include "ESC/Registry.h"
#include "ESC/Components/Transform.h"
#include "ESC/Components/Render.h"
#include "ESC/Components/Camera.h"
#include "ESC/Components/Steering.h"
#include "ESC/Components/Target.h"
#include "Velocity.h"

namespace ECS {

    class UISystem final : public System {
    public:
        UISystem() = default;

        void OnStart(Registry& /*registry*/) override {
            // Habilitar docking.
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            setupStyleChido();
        }

        void
            OnUpdate(Registry& registry, float deltaTime) override {
            ImGuiDockNodeFlags dockspaceFlags =
                ImGuiDockNodeFlags_PassthruCentralNode;

            ImGui::DockSpaceOverViewport(
                0,
                ImGui::GetMainViewport(),
                dockspaceFlags
            );

            DrawOutliner(registry);
            DrawDetails(registry);
        }

        void
            DrawOutliner(Registry& registry) {
            ImGui::Begin("Outliner");
            {
                registry.GetView<ECS::Transform>().Each(
                    [this](ECS::EntityID id, ECS::Transform& /*transform*/) {
                        const std::string label = "Entity " + std::to_string(id);
                        const bool isSelected = (id == selectedEntity);

                        if (ImGui::Selectable(label.c_str(), isSelected))
                            selectedEntity = id;
                    });
            }
            ImGui::End();
        }

        void
            DrawDetails(Registry& registry) {
            ImGui::Begin("Details");
            {
                if (selectedEntity != ECS::NULL_ENTITY && registry.IsAlive(selectedEntity)) {
                    ImGui::Text("Entity %llu", static_cast<unsigned long long>(selectedEntity));
                    ImGui::Separator();

                    if (auto* t = registry.TryComponent<ECS::Transform>(selectedEntity)) {
                        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                            vec2Control("Position", &t->position.x, 0, 120.0f);
                            vec2Control("Rotation", &t->rotation, 1.f, 120.0f);
                            vec2Control("Scale", &t->scale.x, 0.01f, 120.0f);
                            //ImGui::DragFloat2("Position", &t->position.x, 1.f);
                            //ImGui::DragFloat("Rotation", &t->rotation, 1.f);
                            //ImGui::DragFloat2("Scale", &t->scale.x, 0.01f);

                        }

                    }

                    if (auto* r = registry.TryComponent<ECS::Render>(selectedEntity)) {
                        if (ImGui::CollapsingHeader("Render", ImGuiTreeNodeFlags_DefaultOpen)) {
                            ImGui::Checkbox("Visible", &r->visible);

                            // sf::Color es 0..255; ImGui::ColorEdit4 trabaja en 0..1.
                            float color[4] = {
                              r->fillColor.r / 255.f,
                              r->fillColor.g / 255.f,
                              r->fillColor.b / 255.f,
                              r->fillColor.a / 255.f
                            };

                            // Permite modificar el color de relleno del objeto desde el Inspector
                            if (ImGui::ColorEdit4("Fill Color", color)) {
                                r->fillColor = sf::Color(
                                    static_cast<std::uint8_t>(color[0] * 255.f),
                                    static_cast<std::uint8_t>(color[1] * 255.f),
                                    static_cast<std::uint8_t>(color[2] * 255.f),
                                    static_cast<std::uint8_t>(color[3] * 255.f));
                            }
                        }
                    }

                    //Permite activar o desactivar la cámara, modificar el zoom,
                    //ajustar la velocidad de seguimiento y visualizar el objetivo
                    //que la cámara está siguiendo
                    if (auto* cam = registry.TryComponent<ECS::Camera>(selectedEntity)) {
                        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
                            ImGui::Checkbox("Active", &cam->active);
                            ImGui::DragFloat("Zoom", &cam->zoom, 0.01f, 0.05f, 10.f);
                            ImGui::DragFloat("Follow Speed", &cam->followSpeed, 0.1f, 0.f, 50.f);

                            if (cam->followTarget == ECS::NULL_ENTITY)
                                ImGui::Text("Follow Target: (ninguno)");
                            else
                                ImGui::Text("Follow Target : %llu",
                                    static_cast<unsigned long long> (cam->followTarget));
                        }
                    }
                }
                else {
                    ImGui::TextDisabled("Selecciona una entidad en el outliner.");
                }

				// Permite modificar los parámetros del componente Steering, incluyendo el tipo de comportamiento, la velocidad máxima, la fuerza máxima y los parámetros específicos de cada comportamiento.
				//Si el componente Steering está presente en la entidad seleccionada, se muestra un encabezado colapsable con los controles para modificar sus parámetros. Se puede habilitar o deshabilitar el comportamiento de Steering, seleccionar el tipo de comportamiento (Seek, Flee, Arrive, Wander, Pursuit, Obstacle Avoidance) y ajustar la velocidad máxima y la fuerza máxima. Además, se muestran controles adicionales según el tipo de comportamiento seleccionado.
                if (auto* steering = registry.TryComponent<ECS::Steering>(selectedEntity))
                {
                    if (ImGui::CollapsingHeader("Steering", ImGuiTreeNodeFlags_DefaultOpen))
                    {

						// Arreglo de cadenas que representa los nombres de los comportamientos disponibles.
                        const char* behaviors[] =
                        {
                               "Seek",
                               "Flee",
                               "Arrive",
                               "Wander",
                               "Pursuit",
                               "Obstacle Avoidance"
                        };

                        int currentBehavior = static_cast<int>(steering->type);
                        ImGui::Checkbox("Enabled", &steering->enabled);
                        ImGui::BeginDisabled(!steering->enabled);

                        

                        if (ImGui::Combo("Behavior",
                            &currentBehavior,
                            behaviors,
                            IM_ARRAYSIZE(behaviors)))
                        {
                            steering->type =
                                static_cast<ECS::SteeringType>(currentBehavior);

                            if (auto* velocity =
                                registry.TryComponent<ECS::Velocity>(selectedEntity))
                            {
                                velocity->velocity = { 0.f,0.f };
                            }
                        }

                        ImGui::DragFloat(
                            "Max Speed",
                            &steering->maxSpeed,
                            1.0f,
                            0.0f,
                            1000.0f);

                        ImGui::DragFloat(
                            "Max Force",
                            &steering->maxForce,
                            0.5f,
                            0.0f,
                            500.0f);
                        ImGui::EndDisabled();

                        switch (steering->type)
                        {
                        case ECS::SteeringType::Wander:

                            ImGui::Separator();

                            ImGui::DragFloat(
                                "Radius",
                                &steering->wanderRadius,
                                1.f,
                                0.f,
                                500.f);

                            ImGui::DragFloat(
                                "Distance",
                                &steering->wanderDistance,
                                1.f,
                                0.f,
                                500.f);

                            ImGui::DragFloat(
                                "Jitter",
                                &steering->wanderJitter,
                                0.1f,
                                0.f,
                                100.f);

                            break;

                        case ECS::SteeringType::Pursuit:

                            ImGui::Separator();

                            ImGui::DragFloat(
                                "Prediction",
                                &steering->predictionTime,
                                0.05f,
                                0.f,
                                5.f);

                            break;

                        case ECS::SteeringType::ObstacleAvoidance:

                            ImGui::Separator();

                            ImGui::DragFloat(
                                "Look Ahead",
                                &steering->lookAhead,
                                1.f,
                                0.f,
                                300.f);

                            ImGui::DragFloat(
                                "Avoid Force",
                                &steering->avoidForce,
                                1.f,
                                0.f,
                                500.f);

                            ImGui::DragFloat(
                                "Avoid Radius",
                                &steering->avoidRadius,
                                1.f,
                                5.f,
                                300.f);

                            break;
                        }
                    }
                }
            }

           

            if (auto* target = registry.TryComponent<ECS::Target>(selectedEntity))
            {
                if (ImGui::CollapsingHeader("Target", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    if (target->entity == ECS::NULL_ENTITY)
                    {
                        ImGui::Text("Target: Ninguno");
                    }
                    else
                    {
                        ImGui::Text("Target: Entity %llu",
                            static_cast<unsigned long long>(target->entity));
                    }

                    ImGui::Separator();

                    registry.GetView<ECS::Transform>().Each(
                        [&](ECS::EntityID id, ECS::Transform&)
                        {
                            std::string label = "Entity " + std::to_string(id);

                            if (ImGui::Selectable(label.c_str(), id == target->entity))
                            {
                                target->entity = id;
                            }
                        });
                }
            }

            ImGui::End();
        }

        void setupStyleChido()
        {
            ImGuiStyle& style = ImGui::GetStyle();

            style.Alpha = 1.0f;
            style.DisabledAlpha = 0.60f;

            style.WindowPadding = ImVec2(15.0f, 15.0f);
            style.WindowRounding = 10.0f;
            style.WindowBorderSize = 1.0f;
            style.WindowMinSize = ImVec2(20.0f, 20.0f);
            style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
            style.WindowMenuButtonPosition = ImGuiDir_Left;

            style.ChildRounding = 8.0f;
            style.ChildBorderSize = 1.0f;

            style.PopupRounding = 8.0f;
            style.PopupBorderSize = 1.0f;

            style.FramePadding = ImVec2(8.0f, 6.0f);
            style.FrameRounding = 6.0f;
            style.FrameBorderSize = 0.0f;

            style.ItemSpacing = ImVec2(10.0f, 8.0f);
            style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);

            style.CellPadding = ImVec2(8.0f, 6.0f);

            style.IndentSpacing = 22.0f;
            style.ColumnsMinSpacing = 6.0f;

            style.ScrollbarSize = 13.0f;
            style.ScrollbarRounding = 8.0f;

            style.GrabMinSize = 12.0f;
            style.GrabRounding = 8.0f;

            style.TabRounding = 8.0f;
            style.TabBorderSize = 0.0f;

            style.ColorButtonPosition = ImGuiDir_Right;
            style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
            style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

            //========================
            // Colors
            //========================

            style.Colors[ImGuiCol_Text] = ImVec4(0.95f, 0.94f, 0.95f, 1.00f);
            style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.57f, 0.52f, 0.54f, 1.00f);

            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.06f, 0.07f, 1.00f);
            style.Colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.08f, 0.09f, 1.00f);
            style.Colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.08f, 0.09f, 1.00f);

            style.Colors[ImGuiCol_Border] = ImVec4(0.23f, 0.17f, 0.19f, 1.00f);
            style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

            style.Colors[ImGuiCol_FrameBg] = ImVec4(0.14f, 0.11f, 0.12f, 1.00f);
            style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.15f, 0.18f, 1.00f);
            style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.38f, 0.18f, 0.22f, 1.00f);

            style.Colors[ImGuiCol_TitleBg] = ImVec4(0.06f, 0.05f, 0.06f, 1.00f);
            style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.08f, 0.10f, 1.00f);
            style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.06f, 0.05f, 0.06f, 0.85f);

            style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.11f, 0.09f, 0.10f, 1.00f);

            style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.08f, 0.07f, 0.08f, 1.00f);
            style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.15f, 0.18f, 1.00f);
            style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.45f, 0.22f, 0.27f, 1.00f);
            style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.65f, 0.30f, 0.36f, 1.00f);

            style.Colors[ImGuiCol_CheckMark] = ImVec4(0.82f, 0.38f, 0.45f, 1.00f);

            style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.75f, 0.33f, 0.40f, 1.00f);
            style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.90f, 0.45f, 0.52f, 1.00f);

            style.Colors[ImGuiCol_Button] = ImVec4(0.18f, 0.12f, 0.14f, 1.00f);
            style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.42f, 0.18f, 0.24f, 1.00f);
            style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.60f, 0.24f, 0.31f, 1.00f);

            style.Colors[ImGuiCol_Header] = ImVec4(0.18f, 0.12f, 0.14f, 1.00f);
            style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.42f, 0.18f, 0.24f, 1.00f);
            style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.60f, 0.24f, 0.31f, 1.00f);

            style.Colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.18f, 0.20f, 1.00f);
            style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.58f, 0.28f, 0.33f, 1.00f);
            style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.75f, 0.36f, 0.43f, 1.00f);

            style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.18f, 0.20f, 1.00f);
            style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.55f, 0.25f, 0.31f, 1.00f);
            style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.75f, 0.36f, 0.43f, 1.00f);

            style.Colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.09f, 0.10f, 1.00f);
            style.Colors[ImGuiCol_TabHovered] = ImVec4(0.38f, 0.18f, 0.22f, 1.00f);
            style.Colors[ImGuiCol_TabActive] = ImVec4(0.25f, 0.13f, 0.16f, 1.00f);
            style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.09f, 0.08f, 0.09f, 1.00f);
            style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.15f, 0.11f, 0.12f, 1.00f);

            style.Colors[ImGuiCol_PlotLines] = ImVec4(0.85f, 0.50f, 0.55f, 1.00f);
            style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.65f, 0.70f, 1.00f);

            style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.78f, 0.34f, 0.40f, 1.00f);
            style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.95f, 0.50f, 0.56f, 1.00f);

            style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.11f, 0.09f, 0.10f, 1.00f);
            style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.24f, 0.17f, 0.19f, 1.00f);
            style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.17f, 0.12f, 0.14f, 1.00f);

            style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.08f, 0.07f, 0.08f, 1.00f);
            style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.11f, 0.09f, 0.10f, 1.00f);

            style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.60f, 0.24f, 0.31f, 0.70f);

            style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.90f, 0.42f, 0.50f, 1.00f);

            style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.90f, 0.42f, 0.50f, 1.00f);
            style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.90f, 0.42f, 0.50f, 1.00f);

            style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.05f, 0.04f, 0.05f, 0.75f);
            style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.05f, 0.04f, 0.05f, 0.75f);
        }

        void vec2Control(const std::string& label,
            float* values,
            float resetValues,
            float columnWidth)
        {
            ImGuiIO& io = ImGui::GetIO();
            auto boldFont = io.Fonts->Fonts[0];

            ImGui::PushID(label.c_str());

            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, columnWidth);

            ImGui::AlignTextToFramePadding();
            ImGui::Text("%s", label.c_str());

            ImGui::NextColumn();

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6.0f, 6.0f));

            float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.5f;
            ImVec2 buttonSize(lineHeight + 4.0f, lineHeight);

            float fullWidth = ImGui::CalcItemWidth();
            float itemWidth = (fullWidth - 20.0f) * 0.5f;

            //-------------------- X --------------------//

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.50f, 0.16f, 0.22f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.66f, 0.22f, 0.30f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.76f, 0.30f, 0.38f, 1.0f));

            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

            ImGui::PushFont(boldFont);

            if (ImGui::Button("X", buttonSize))
                values[0] = resetValues;

            ImGui::PopFont();

            ImGui::PopStyleVar();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();

            ImGui::PushItemWidth(itemWidth);

            ImGui::DragFloat("##X", &values[0], 0.05f, 0.0f, 0.0f, "%.2f");

            ImGui::PopItemWidth();

            ImGui::SameLine();

            //-------------------- Y --------------------//

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.38f, 0.18f, 0.22f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.56f, 0.24f, 0.30f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.70f, 0.32f, 0.38f, 1.0f));

            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

            ImGui::PushFont(boldFont);

            if (ImGui::Button("Y", buttonSize))
                values[1] = resetValues;

            ImGui::PopFont();

            ImGui::PopStyleVar();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();

            ImGui::PushItemWidth(itemWidth);

            ImGui::DragFloat("##Y", &values[1], 0.05f, 0.0f, 0.0f, "%.2f");

            ImGui::PopItemWidth();

            ImGui::PopStyleVar();

            ImGui::Columns(1);

            ImGui::PopID();
        }



    private:
        // Entidad seleccionada en el outliner (NULL_ENTITY = ninguna).
        ECS::EntityID selectedEntity = ECS::NULL_ENTITY;
    };

};