#pragma once
#include "Prerequisites.h"

//ECS :: Component/Render.h
//
//Render -- Componente de DATOS PUROS que guarda la forma 
//dibujable (sf:: Shape) de una entidad.
//
//Se almacena en un ComponentPool<Render>. Usa shared_ptr
//(movible y copiable) para ser compatible con el pool,
// a diferncia de Cshape, que tiene el move suprimido.
//
//La posicio/rotacion/escala NO vivven aqui: vienen del
//componente Transform y las aplica el RenderSystme cada frame.
//=============================================================

namespace ECS {
	struct Render
	{
		std::shared_ptr<sf::Shape> shape;			//la forma de dibujar
		std::shared_ptr<sf::Texture> texture;		//textura opcional
		sf::Color fillColor{ sf::Color::White };	//Color relleno
		bool visible{ true };						//permite ocultar sin quitar  el componente

		Render() = default;
		explicit Render(std::shared_ptr<sf::Shape> s,
			sf::Color color = sf::Color::White) noexcept
			: shape(std::move(s)), fillColor(color) {

		}

		bool SetTexture(const std::string& path, bool resetRect = true) {
			if (!shape) return false;
			auto text = std::make_shared<sf::Texture>();
			if (!text->loadFromFile(path)) return false;
			texture = std::move(text);
			shape->setTexture(texture.get(), resetRect);
			return true;
		}

		void SetTexture(std::shared_ptr < sf::Texture> text, bool resetRect = true) {
			if (!shape) return;
			texture = std::move(text);
			shape->setTexture(texture ? texture.get() : nullptr, resetRect);
		}

		void ClearTexture() {
			if (shape) shape->setTexture(nullptr);
			texture.reset();
		}

		[[nodiscard]] static Render
			Make(ShapeType type, sf::Color color = sf::Color::White,
				const std::string& texturePath = "") { //sprite opcional
			std::shared_ptr<sf::Shape> s;
			switch (type) {
			case CIRCLE: {
				auto c = std::make_shared<sf::CircleShape>(50.f);
				c->setOrigin({ 50.f, 50.f });
				s = c;
				break;
			}
			case RECTANGLE: {
				sf::Vector2f size{ 100.f, 50.f };
				auto r = std::make_shared<sf::RectangleShape>(size);
				r->setOrigin(size / 2.f);
				s = r;
				break;
			}
			case TRIANGLE: {
				auto t = std::make_shared<sf::ConvexShape>(3);
				t->setPoint(0, { 0.f, 0.f });
				t->setPoint(1, { 100.f, 0.f });
				t->setPoint(2, { 50.f, 100.f });
				t->setOrigin({ 50.f, 50.f });
				s = t;
				break;
			}
			case POLYGON: {
				auto p = std::make_shared<sf::ConvexShape>(5);
				p->setPoint(0, { 0.f, 0.f });
				p->setPoint(1, { 100.f, 0.f });
				p->setPoint(2, { 120.f, 50.f });
				p->setPoint(3, { 50.f, 100.f });
				p->setPoint(4, { -20.f, 50.f });
				p->setOrigin({ });
				s = p;
				break;
			}
			case LINE: {
				auto l = std::make_shared<sf::RectangleShape>(sf::Vector2f(200.f, 4.f));
				l->setOrigin({}); 
				s = l;
				break;
			}
			default:
				break;
			}
			if (s) s->setFillColor(color); 
				Render render{ s, color };
			//si se paso una ruta, intenta cargar el sprite (silencioso si falla)
			if (!texturePath.empty())
				render.SetTexture(texturePath);
			return render;

		}
	
	
	};
}
