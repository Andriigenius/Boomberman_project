#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Entity.h"

class AnimatedExplosion : public Entity {
private:
    std::vector<sf::Texture*> frames;  // Указатели на текстуры (принимаются извне)
    sf::Sprite sprite;
    float frameTime;
    float timer;
    int currentFrame;
    int totalFrames;
    bool finished;
    sf::Vector2f position;
    float tileSize;

public:
    AnimatedExplosion(const std::vector<sf::Texture*>& frameTextures, sf::Vector2f pos, float tileSize = 60.f);

    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    sf::Vector2i getTilePosition() const override;
    bool isFinished() const;
};
