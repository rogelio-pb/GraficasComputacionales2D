#pragma once

//ESC:: System.h
//Clase base para todos los sistemas d emotor
//
//Unsistema contienene unicamente logica, nunca datos
//Los datos viven en los componentes.
//
//Ciclo de vida:
//OnStart--> llamado una vez al registrar el sistema
//OnUpdate --> llamado ca frame
//OnStop --> llamado l destruir o desregistrar ek sistema 

namespace ECS {
	class Registry;
	/**
 * @brief clase base para crear sistemas.
 *
 * los sistemas contienen la logica del juego y trabajan
 * con los datos que están guardados en los componentes
 */
	class
		System {
	public:
		virtual ~System() = default;
		virtual void OnStart(Registry& /*registry*/) {}
		/**
	 * @brief actaliza el sistema
	 * @param registry Registro pricipa
	 * @param deltaTime Tiempo entre frames}
	 */

		virtual void OnUpdate(Registry& registry, float deltaTime) = 0;

		virtual void OnDestroy(Registry& /*registry*/) {}

		void SetEnabled(bool enabled) noexcept { m_enabled = enabled; }
		/**
		 * @brief Rtegresa si el sistema está activo
		 * @return true si está activo
		 */
		[[nodiscard]] bool IsEnabled() const noexcept { return m_enabled; }


	private:
		/**
		 * @brief indica si el sitema esta habilitdo
		 */
		bool m_enabled = true;
	};
}
