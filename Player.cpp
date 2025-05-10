#include "Player.h"
 
extern const int TILE_SIZE; //гдето в коде имееться данная перменная наприммер мейн.спп

Player::Player(sf::Vector2f startPos, sf::Vector2f size) {
    position = startPos;
    previousPosition = startPos;
    direction = { 0.f, 0.f };
    speed = 150.f;
    shape.setSize(size);
    shape.setFillColor(sf::Color::Green);
}

void Player::Moveset() {                   //движение
    direction = { 0.f, 0.f };
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) direction.y = -1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) direction.y = 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) direction.x = -1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) direction.x = 1.f;
}

void Player::update(float dt) {     // плавное передвижение
    lastDt = dt;
    previousPosition = position;
    position += direction * speed * dt;
}

void Player::handleCollision(const std::vector<std::string>& map) {           //проверка на столкновения
    sf::Vector2f size = shape.getSize();

    // Проверка 4 углов
    sf::Vector2f points[4] = {
        position,
        position + sf::Vector2f(size.x, 0.f),
        position + sf::Vector2f(0.f, size.y),
        position + size
    };

    for (auto& p : points) {
        int x = static_cast<int>(p.x) / TILE_SIZE;
        int y = static_cast<int>(p.y) / TILE_SIZE;

        if (y < 0 || y >= map.size() || x < 0 || x >= map[y].size()) continue;

        if (map[y][x] != ' ') {
            position = previousPosition; // откат
            break;
        }
    }
}

void Player::draw(sf::RenderWindow& window) {
    shape.setPosition(position);
    window.draw(shape);
}

sf::Vector2i Player::getTilePosition() const {
    return sf::Vector2i(position.x / TILE_SIZE, position.y / TILE_SIZE);
}
