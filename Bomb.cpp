#include "Bomb.h"

Bomb::Bomb(sf::Vector2f pos, float delaySec, float tileSz, std::function<void(sf::Vector2i)> onExplodeCallback)
    : position(pos), delay(delaySec), timer(0.f), exploded(false), tileSize(tileSz), onExplode(onExplodeCallback) 
{
    shape.setSize(sf::Vector2f(tileSize, tileSize));
    shape.setFillColor(sf::Color::Red);
    shape.setOutlineColor(sf::Color::Red);
    shape.setOutlineThickness(1.f);
    shape.setPosition(position);
}

void Bomb::update(float dt) {
    if (exploded) return;

    timer += dt;
    if (timer >= delay) {
        exploded = true;

        if (onExplode) {
            sf::Vector2i tile = {
                static_cast<int>(position.x / tileSize),
                static_cast<int>(position.y / tileSize)
            };
            onExplode(tile);
        }
    }
}

void Bomb::draw(sf::RenderWindow& window) {
    if (!exploded) {
        window.draw(shape);
    }
}
bool Bomb::isExploded() const {
    return exploded;
}
sf::Vector2i Bomb::getTilePosition() const {
    return sf::Vector2i(
        static_cast<int>(position.x / tileSize),
        static_cast<int>(position.y / tileSize)
    );
}
