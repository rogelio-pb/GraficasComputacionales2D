#pragma once
#include "Prerequisites.h"

class
	Window {
public:
	Window() = default;
	Window(int width, int height, const std::string& title);
	~Window()=default;

	//void
	//handleEvents(EngineGUI& gui);

	bool
		isOpen() const;


	/**
	 * @brief Limpia la ventana con un color.
	 * @param color Color de fondo.
	 */

	void
		clear(const sf::Color& color = sf::Color(0, 0, 0, 255));

	/**
	 * @brief Dibuja un objeto en la ventana.
	 * @param drawable Objeto a dibujar.
	 * @param states Estados de renderizado.
	 */
	void
		draw(const sf::Drawable& drawable,
			const sf::RenderStates& states = sf::RenderStates::Default);

	/**
	 * @brief Muestra el contenido en pantalla.
	 */
	void
		display();

	/**
	 * @brief Cierra la ventana.
	 */

	void
		close();

	/**
	 * @brief Actualiza datos de la ventana.
	 */
	void
		update();


	/**
	 * @brief Función de renderizado.
	 */
	void
		render();
	/**
	 * @brief Libera los recursos de la ventana.
	 */
	void
		destroy();

public:
	std::unique_ptr<sf::RenderWindow> m_window=nullptr;
private:
	sf::View m_view;
	sf::Time deltaTime;
	sf::Clock clock;
};
