#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Enemy : public Entity {
private:
    sf::Vector2f position;
    sf::Vector2f direction;
    sf::RectangleShape shape;
    float speed;
    float moveTimer;
    float moveDelay;
    const std::vector<std::string>& map;

public:
    Enemy(sf::Vector2f startPos, const std::vector<std::string>& mapRef);

    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    sf::Vector2i getTilePosition() const override;
    sf::FloatRect getBounds() const;
    bool isDead = false;
};
