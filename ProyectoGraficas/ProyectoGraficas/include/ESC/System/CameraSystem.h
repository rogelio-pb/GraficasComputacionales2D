/**
 * @file CameraSystem.h
 * @brief Sistema encargado de actualizar la cámara del juego.
 */
#pragma once
#include "ESC/System.h"
#include  "ESC/Registry.h"
#include "ESC/Components/Transform.h"
#include "ESC/Components/Camera.h"
#include "Core/Window.h"
#include <cmath>

namespace ECS{
	
	class CameraSystem final : public System {
	public:
		/**
	   * @brief Crea el sistema de cámara.
	   *
	   * @param window Ventana sobre la que se aplicará la vista de la cámara.
	   */
		explicit CameraSystem(Window& window) noexcept
			: m_window(window) {
		}

		/**
	   * @brief Actualiza la posición de la cámara
	   * Si la cámara tiene un objetivo válido, se mueve
	   * gradualmente hacia él utilizando la velocidad de seguimiento
	   * Después aplica la posición y el nivel de zoom a la ventana
	   */
		void OnUpdate(Registry& registry, float deltaTime) override {
			registry.GetView<Transform, Camera>().Each(
				[this, &registry, deltaTime](EntityID, Transform& camT, Camera& cam) {
					
					//si la cámara no esta activa no realiza ninguna actualización
					if (!cam.active) return;

					if (cam.followTarget != NULL_ENTITY &&
						registry.IsAlive(cam.followTarget)) {
						if (auto* targetT = registry.TryComponent<Transform>(cam.followTarget)) {
							const float t = 1.f - std::exp(-cam.followSpeed * deltaTime);
							camT.position += (targetT->position - camT.position) * t;
						}
					}
					// Aplica la posición y el zoom de la cámara a la ventana
					m_window.applyCameraView(camT.position, cam.zoom);
				});
				}
	private:
		Window& m_window;
			
	};
}