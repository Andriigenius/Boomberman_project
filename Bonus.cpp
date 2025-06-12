#include "Bonus.h"
#include <iostream>

sf::Texture Bonus::s_fireTexture;
sf::Texture Bonus::s_speedTexture;
bool        Bonus::s_texturesLoaded = false;

Bonus::Bonus(sf::Vector2f pos, BonusType t)
    : type(t)
{
    if (!s_texturesLoaded) {
        if (!s_fireTexture.loadFromFile("textures/RadiusBonus.png"))
            std::cerr << "Failed to load textures/bonus_fire.png\n";
        if (!s_speedTexture.loadFromFile("textures/SpeedBonus.png"))
            std::cerr << "Failed to load textures/bonus_speed.png\n";
        s_texturesLoaded = true;
    }

    // в зависимости от типа навешиваем готовую статическую текстуру
    switch (type) {
    case BonusType::FirePower: sprite.setTexture(s_fireTexture);  break;
    case BonusType::SpeedUp:   sprite.setTexture(s_speedTexture); break;
    }

    // позиционируем и (опционально) масштабируем под TILE_SIZE
    sprite.setPosition(pos);
    float scaleX = 60.f / sprite.getTexture()->getSize().x;
    float scaleY = 60.f / sprite.getTexture()->getSize().y;
    sprite.setScale(scaleX, scaleY);
}

void Bonus::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::Vector2i Bonus::getTilePosition() const {
    return sf::Vector2i(
        int(sprite.getPosition().x) / 60,
        int(sprite.getPosition().y) / 60
    );
}

sf::FloatRect Bonus::getBounds() const {
    return sprite.getGlobalBounds();
}

BonusType Bonus::getType() const {
    return type;
}
