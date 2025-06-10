#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

enum class EDir { Down, Up, Left, Right };

class Enemy : public Entity {
private:

    // для анимации: 2 кадра на каждое направление
    sf::Texture texturesDown[2];
    sf::Texture texturesUp[2];
    sf::Texture texturesLeft[2];
    sf::Texture texturesRight[2];

    sf::Sprite sprite;         // вместо shape для рендера и коллизий

    // анимация
    bool isMoving = false;
    EDir animDir = EDir::Down;
    int frame = 0;
    float frameTimer = 0.f;
    const float frameDuration = 0.3f; // секунды на кадр

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
