#pragma once
#include "Prerequisites.h"
class Window;
/**
 * @brief Clase para manejar figuras de SFML.
 *
 * Permite crear y dibujar diferentes tipos de figuras.
 */
class CShape
{
public:
	CShape()=default;
	explicit CShape(ShapeType shapeType);
	~CShape() = default;
	void
		draw(Window& window);

		sf::Shape* 
		getShape();
	private:
		static std::unique_ptr<sf::Shape> createShape(ShapeType shapeType);

	private:
		std::unique_ptr<sf::Shape> m_shape = nullptr;
		ShapeType m_shapeType;
	

};