/**
 * @file main.cpp
 * @brief punto de entrada principal de la aplicacion
 * ste archivo crea una ventana graficainstancia una figura
 * de tipo cíiculo y ejecuta el ciclo principal de renderizado
 * hasta que el usuario cierre la ventana
 */

#include "Prerequisites.h"
#include "Core/Window.h"
#include "Core/CShape.h"
Window* g_window = nullptr;
/**
 * @brief ojeto gráfico de tipo circulo
 *representa la figura que será dibujada en la ventana durante
 * cada iteración del ciclo principal
 */
CShape Circle(ShapeType::CIRCLE);

void destroy() {
	SAFE_PTR_RELEASE(g_window);
}
/**
 * @brief Función principal de la aplicación
 * Crea una ventana
 * Procesa eventos del sistem
 * Limpia la pantalla en cada fotograma
 * Dibuja un crculo en la ventan
 * Actualiza la pantalla
 *Libera los recursos antes de finalizar
 * @retrn int Retorna 0 si la ejecución finaliza correctamente
 */

int main()
{
    // create the window
    g_window = new Window(800, 600, "My window");

    //shape.setFillColor(sf::Color(100, 250, 50));
    // run the program as long as the window is open
    while (g_window->isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = g_window->m_window->pollEvent())
        {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>()) {
                g_window->close();

            }
        }



        g_window->clear(sf::Color::Black);


        // draw everything here...
        Circle.draw(*g_window);
        //g_window->draw(shape);

        // end the current frame
        g_window->display();
    }
    destroy();
    return 0;
}