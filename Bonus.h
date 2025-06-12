#pragma once
#include <SFML/Graphics.hpp>
#include "Entity.h"

enum class BonusType {
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

    static sf::Texture s_fireTexture;
    static sf::Texture s_speedTexture;
    static bool        s_texturesLoaded;
private:
    BonusType type;
    sf::Sprite sprite;

    
};