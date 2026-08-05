#include "Core/Window.h"

Window::Window(int width, int height, const std::string& title)
{
	m_width = width;
	m_height = height;
	m_title = title;
	//configuración del contexto OpenGL
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antiAliasingLevel = 4;

	m_window = std::make_unique<sf::RenderWindow>(sf::VideoMode({ static_cast<unsigned int>(width),
		static_cast<unsigned int>(height) }),
		title,
		sf::Style::Default,
		sf::State::Windowed,
		settings
	);

	if (m_window->isOpen()) {
		//Usa VSync o limite de FPS segun tu necesidad
		m_window->setVerticalSyncEnabled(true);
		//m_window->setFramerateLimit(60);

		handleResize(m_window->getSize());
		const sf::ContextSettings actualSettings =
			m_window->getSettings();

		MESSAGE("Window", "Window", , "MSAA disponible: " + std::to_string(actualSettings.antiAliansingLevel) + "x");

		MESSAGE("Window", "Window", "Window created successfully");

	}
		else {
		ERROR("Window", "window", "Failed to create window");
	}
}


	bool
		Window::isOpen() const {
	// Check that window is not null
		if (m_window) {
		return m_window && m_window->isOpen();
			}
		else {
		ERROR("Window", "isOpen", "Window is null");
		return false;
			}
}
	
/**
 * @brief limpia la ventana con un color
 * @param color Color de fondo
 */
	void
		Window::clear(const sf::Color & color) {
		if (m_window) {
			m_window->clear(color);
		}
		else {
			ERROR("Window", "clear", "Window is not initialized");


		}
	}
	/**
	 * @brief Dibuja un objeto en la ventana
	 * @param drawable Objeto a dibujar
	 * @param States Estados de renderizado
	 */

	void
		Window::draw(const sf::Drawable & drawable, const sf::RenderStates & States) {
		if (m_window) {
			m_window->draw(drawable, States);

		}
		else {
			ERROR("Window", "draw", "Window is not initialized");
		}
	}

	void Window::setMSAALevel(unsigned int level)
	{
		if (!m_window) {
			return;
		}
		const sf::Vector2u currentSize = m_window->getSize();
		const sf::Vector2i currentPosition = m_window->getPosition();
		const sf :: View currentView = m_window->getView();

		sf::ContextSettings settings;
		settings.depthBits = 24;
		settings.stencilBits = 8;
		settings.antiAliasingLevel = level;

		m_window->create(
			sf::VideoMode(currentSize),
			m_title,
			sf::Style::Default,
			sf::State::Windowed,
			settings
		);
		m_window->setPosition(currentPosition);
		m_window->setView(currentView);
		m_window->setVerticalSyncEnabled(true);

		const sf::ContextSettings actualSettings =
			m_window->getSettings();

		MESSAGE("Window", "Window", , "MSAA disponible: " + std::to_string(actualSettings.antiAliansingLevel) + "x");

		//////////////////
	}
	void
		Window::display() {
		if (m_window) {
			m_window->display();
		}
		else {
			ERROR("Window", "display", "Window is null");
		}
	}

	void
		Window::close() {
		if (m_window) {
			m_window->close();
		}
		else {
			ERROR("Window", "close", "Window is not initialized");


		}
	}

	void
		Window::handleResize(const sf::Vector2u& size) {
		if (!m_window) {
			ERROR("Window", "handleResize", "Window is null");
			return;
		}
		//vista 1.1 con el tamanio de la ventana
		//Centro de la vista en (0,0)
		//el centro de la pnatalla 
		const sf::Vector2f fSize(static_cast<float>(size.x),
			static_cast<float>(size.y));
		m_baseViewSize = fSize;
		m_view.setSize(fSize);
		m_view.setCenter({ 0.f, 0.f });
		m_window->setView(m_view);
	}

	void
		Window::applyCameraView(const sf::Vector2f& center, float zoom, float rotationDeg) {
		if (!m_window) {
			ERROR("Window", "apllyCmaeraView", "Window is null");
			return;
		}
		if (zoom <= 0.f) zoom = 1.f;

		m_view.setSize(m_baseViewSize / zoom);
		m_view.setCenter(center);
		m_view.setRotation(sf::degrees(rotationDeg));
		m_window->setView(m_view);

	}


		void
			Window::update() {
			deltaTime = clock.restart();
		}

		void
			Window::render() {}
		void
			Window::destroy() {
			m_window.reset();
}