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

void Player::update(float dt) {
    lastDt = dt;
    previousPosition = position;
}


void Player::handleCollision(const std::vector<std::string>& map) {
    sf::FloatRect bounds = shape.getGlobalBounds();

    // Попытка движения по X
    sf::Vector2f tryX = position;
    tryX.x += direction.x * speed * lastDt;
    sf::FloatRect nextBoundsX(tryX, shape.getSize());

    bool blockedX = false;
    for (int y = 0; y < map.size(); ++y) {
        for (int x = 0; x < map[y].size(); ++x) {
            if (map[y][x] != ' ') {
                sf::FloatRect tileRect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
                if (nextBoundsX.intersects(tileRect)) {
                    blockedX = true;
                    break;
                }
            }
        }
        if (blockedX) break;
    }

    if (!blockedX)
        position.x = tryX.x;

    // Попытка движения по Y
    sf::Vector2f tryY = position;
    tryY.y += direction.y * speed * lastDt;
    sf::FloatRect nextBoundsY(tryY, shape.getSize());

    bool blockedY = false;
    for (int y = 0; y < map.size(); ++y) {
        for (int x = 0; x < map[y].size(); ++x) {
            if (map[y][x] != ' ') {
                sf::FloatRect tileRect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
                if (nextBoundsY.intersects(tileRect)) {
                    blockedY = true;
                    break;
                }
            }
        }
        if (blockedY) break;
    }

    if (!blockedY)
        position.y = tryY.y;
}


sf::FloatRect Player::getBounds() const
{
        return shape.getGlobalBounds();
}

void Player::draw(sf::RenderWindow& window) {
    shape.setPosition(position);
    window.draw(shape);
}

sf::Vector2i Player::getTilePosition() const {
    return sf::Vector2i(position.x / TILE_SIZE, position.y / TILE_SIZE);
}


void Player::setPos(sf::Vector2f pos)
{
    position = pos;
    shape.setPosition(position);
}
