#pragma once
#include"ESC/System.h"
#include "ESC/Registry.h"
#include"ESC/Components/Transform.h"
#include"ESC/Components/Render.h"
#include "Core/Window.h"

//ECS :: Systems/RenderSystem.h
//
//LOGICA PURA: recorre todas las entidades que tengan a la vez
//Transform + ender, vuelca el transform sobre la sf::shape
//y la dibuja en la ventana.
//
//Debe llamarse Dentro del frame, entre Window::clear()
//y Window::display().
//=============================================================

namespace ECS {
	class RenderSystem final : public System {
	public:
		explicit RenderSystem(Window& window) noexcept
			: m_window(window) {
		}

		void OnUpdate(Registry& registry, float /*deltaTime*/) override {
			registry.GetView<Transform, Render>().Each(
				[this](EntityID /*entity*/, Transform& t, Render& r) {
					if (!r.shape || !r.visible) return;

					r.shape->setPosition(t.position);
					r.shape->setRotation(sf::degrees(t.rotation));
					r.shape->setScale(t.scale);
					r.shape->setFillColor(r.fillColor);

					m_window.draw(*r.shape);

				});
		}
	private:
		Window& m_window;
	};
}


