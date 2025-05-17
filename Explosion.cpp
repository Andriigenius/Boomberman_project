#include "Explosion.h"
#include <iostream>

Explosion::Explosion(sf::Vector2f position, float tileSize) {
    shape.setSize(sf::Vector2f(tileSize, tileSize));
    shape.setFillColor(sf::Color(255, 165, 0)); // оранжевый
    shape.setPosition(position);
}

void Explosion::update(float dt) {
    timer += dt;
}

void Explosion::draw(sf::RenderWindow& window) {
    if (timer < lifetime)
        window.draw(shape);
}

sf::Vector2i Explosion::getTilePosition() const {
    sf::Vector2f pos = shape.getPosition();
    return { static_cast<int>(pos.x / shape.getSize().x), static_cast<int>(pos.y / shape.getSize().y) };
}

bool Explosion::isFinished() const {
    return timer >= lifetime;
}
