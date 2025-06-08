#include "Bonus.h"

Bonus::Bonus(sf::Vector2f pos, BonusType t) : type(t) {
    shape.setSize(sf::Vector2f(60.f, 60.f));
    shape.setPosition(pos);

    switch (type) {
    case BonusType::FirePower:
        shape.setFillColor(sf::Color::Red);
        break;
    case BonusType::SpeedUp:
        shape.setFillColor(sf::Color::Green);
        break;
    }
}

void Bonus::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

sf::Vector2i Bonus::getTilePosition() const {
    return sf::Vector2i(
        static_cast<int>(shape.getPosition().x) / 60,
        static_cast<int>(shape.getPosition().y) / 60
    );
}

sf::FloatRect Bonus::getBounds() const {
    return shape.getGlobalBounds();
}

BonusType Bonus::getType() const {
    return type;
}