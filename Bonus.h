#pragma once
#include <SFML/Graphics.hpp>
#include "Entity.h"

enum class BonusType {
    ExtraBomb,
    FirePower,
    SpeedUp
};

class Bonus : public Entity {
public:
    Bonus(sf::Vector2f pos, BonusType t);
    void update(float dt) override {} // бонус не двигается
    void draw(sf::RenderWindow& window) override;
    sf::Vector2i getTilePosition() const override;

    sf::FloatRect getBounds() const;
    BonusType getType() const;

private:
    BonusType type;
    sf::RectangleShape shape;
};