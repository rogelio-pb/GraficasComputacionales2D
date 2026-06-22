#pragma once
#include "Prerequisites.h"

//ECS :: C
namespace ECS {
	struct Transform{

		sf::Vector2f position{ 0.f, 0.f };

		float rotation{ 0.f };

		sf::Vector2f scale{ 1.f, 1.f };

		Transform() = default;
		explicit Transform(sf::Vector2f pos,
							float rot=0.f,
							sf::Vector2f scl = {1.f, 1.f}) noexcept
			: position(pos), rotation(rot), scale(scl){
		}

		void Translate(sf::Vector2f delta) noexcept { position += delta;}
		void Rotate(float degrees) noexcept { rotation += degrees; }
	};
}
