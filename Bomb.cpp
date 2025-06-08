#include "Bomb.h"

Bomb::Bomb(sf::Vector2f pos, float delaySec, float tileSz, std::function<void(sf::Vector2i)> onExplodeCallback, sf::Texture& bombTexture)
    : position(pos), delay(delaySec), timer(0.f), exploded(false), tileSize(tileSz), onExplode(onExplodeCallback) 
{

    sprite.setTexture(bombTexture);
    sprite.setPosition(position);
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
        window.draw(sprite);
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
