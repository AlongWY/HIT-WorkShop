#include "DisplayCanvas.h"

void DisplayCanvas::OnUpdate()
{
    OnKeyDown(Cemera);
    m_World.setPos(Cemera);
    clear(sf::Color::White);
    draw(m_World);
};

void DisplayCanvas::OnKeyDown(sf::Vector2f& Cemera)
{
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        Cemera.y -= 0.1;
    } else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        Cemera.y += 0.1;
    } else if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        Cemera.x -= 0.1;
    } else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        Cemera.x += 0.1;
    } else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        Cemera.x = 0;
        Cemera.y = 0;
        sf::View view = getView();
        view.setSize(900, 720);
        view.setCenter(0, 0);
        setView(view);
    }
}
