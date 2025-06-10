#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

enum class Direction { Up, Down, Left, Right };

class Player : public Entity {
private:
    sf::Texture texturesDown[2];
    sf::Texture texturesUp[2];
    sf::Texture texturesLeft[2];
    sf::Texture texturesRight[2];

    sf::Sprite sprite;
    sf::Texture textureUp, textureDown, textureLeft, textureRight;


    Direction currentDirection = Direction::Down;
    bool isMoving = false;

    int frame = 0;
    float frameTimer = 0.f;
    const float frameDuration = 0.2f;

    sf::RectangleShape shape;
    sf::Vector2f direction;
    sf::Vector2f position;
    float speed;
    float lastDt = 0.f;
    sf::Vector2f previousPosition;

    int explosionRadius = 1;
public:
    void loadTextures(); // загружает текстуры

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

