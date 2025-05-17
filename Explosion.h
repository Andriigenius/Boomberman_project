#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>

class Explosion : public Entity {
private:
    sf::RectangleShape shape;
    float timer = 0.f;
    float lifetime = 0.5f;

public:
    Explosion(sf::Vector2f position, float tileSize);

    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    sf::Vector2i getTilePosition() const override;
    bool isFinished() const;
};
