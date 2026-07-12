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
		explicit CameraSystem(Window& window) noexcept
			: m_window(window) {
		}

		void OnUpdate(Registry& registry, float deltaTime) override {
			registry.GetView<Transform, Camera>().Each(
				[this, &registry, deltaTime](EntityID, Transform& camT, Camera& cam) {
					if (!cam.active) return;

					if (cam.followTarget != NULL_ENTITY &&
						registry.IsAlive(cam.followTarget)) {
						if (auto* targetT = registry.TryComponent<Transform>(cam.followTarget)) {
							const float t = 1.f - std::exp(-cam.followSpeed * deltaTime);
							camT.position += (targetT->position - camT.position) * t;
						}
					}
					m_window.applyCameraView(camT.position, cam.zoom);
				});
				}
	private:
		Window& m_window;
			
	};
}