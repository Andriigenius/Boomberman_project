#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Player : public Entity {
private:
    sf::RectangleShape shape;
    sf::Vector2f direction;
    sf::Vector2f position;
    float speed;
    float lastDt = 0.f;
    sf::Vector2f previousPosition;

    int explosionRadius = 1;
public:
    void setPos(sf::Vector2f pos);
    bool isDead = false;
    Player(sf::Vector2f startPos, sf::Vector2f size);
    void Moveset();
    void update(float dt);
    void draw(sf::RenderWindow& window) override;
    sf::Vector2i getTilePosition() const override;
    void handleCollision(const std::vector<std::string>& map);
    sf::FloatRect getBounds() const;

    void setFireRadius(int radius);
    void setSpeed(float speedy);

    int getFireRadius() const;
    float getSpeed() const;
};

