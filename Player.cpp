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
    isMoving = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        currentDirection = Direction::Up;
        direction.y = -1.f;
        isMoving = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        currentDirection = Direction::Down;
        direction.y = 1.f;
        isMoving = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        currentDirection = Direction::Left;
        direction.x = -1.f;
        isMoving = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        currentDirection = Direction::Right;
        direction.x = 1.f;
        isMoving = true;
    }

}

void Player::update(float dt) {
    lastDt = dt;
    previousPosition = position;

    if (isMoving) {
        frameTimer += dt;
        if (frameTimer >= frameDuration) {
            frameTimer = 0.f;
            frame = (frame + 1) % 2; // 2 кадра
        }
    }
    else {
        frame = 0;
    }

    // Обновляем текстуру в зависимости от направления и кадра
    switch (currentDirection) {
    case Direction::Down:  sprite.setTexture(texturesDown[frame]); break;
    case Direction::Up:    sprite.setTexture(texturesUp[frame]); break;
    case Direction::Left:  sprite.setTexture(texturesLeft[frame]); break;
    case Direction::Right: sprite.setTexture(texturesRight[frame]); break;
    }

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
    sprite.setPosition(position);
    shape.setPosition(position);
    window.draw(sprite);
}

sf::Vector2i Player::getTilePosition() const {
    return sf::Vector2i(position.x / TILE_SIZE, position.y / TILE_SIZE);
}


void Player::setPos(sf::Vector2f pos)
{
    position = pos;
    shape.setPosition(position);
}

void Player::setFireRadius(int radius) {
    if (explosionRadius < 10)
    explosionRadius = radius;
}

void Player::setSpeed(float speedy) {
    if(speedy < 251)
    speed = speedy;
}

int Player::getFireRadius() const
{
    return explosionRadius;
}

float Player::getSpeed() const
{
    return speed;
}

void Player::loadTextures() {
    texturesDown[0].loadFromFile("textures/LookS1.png");
    texturesDown[1].loadFromFile("textures/LookS2.png");

    texturesUp[0].loadFromFile("textures/LookB1.png");
    texturesUp[1].loadFromFile("textures/LookB2.png");

    texturesLeft[0].loadFromFile("textures/lookL1.png");
    texturesLeft[1].loadFromFile("textures/lookL2.png");

    texturesRight[0].loadFromFile("textures/lookR1.png");
    texturesRight[1].loadFromFile("textures/lookR2.png");

    sprite.setTexture(texturesDown[0]); // по умолчанию вниз
}
