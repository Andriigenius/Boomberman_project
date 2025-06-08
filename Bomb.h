#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <functional>

class Bomb : public Entity {
private:
    sf::Vector2f position;
    float timer;
    float delay;
    bool exploded;
    float tileSize;
    sf::RectangleShape shape;
    sf::Sprite sprite;
    std::function<void(sf::Vector2i)> onExplode;

public:
    Bomb(sf::Vector2f pos, float delaySec, float tileSize, std::function<void(sf::Vector2i)> onExplodeCallback, sf::Texture& bombTexture);

    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    sf::Vector2i getTilePosition() const override;

    bool isExploded() const;
};

