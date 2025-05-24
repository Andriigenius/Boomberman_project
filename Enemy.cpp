#include "Enemy.h"
#include <cstdlib>
#include <ctime>
extern const int TILE_SIZE;

Enemy::Enemy(sf::Vector2f startPos, const std::vector<std::string>& mapRef)
    : position(startPos), map(mapRef), speed(60.f), moveTimer(0.f), moveDelay(1.f) {
    shape.setSize({ 60.f, 60.f });
    shape.setFillColor(sf::Color::Red);
    shape.setPosition(position);
    direction = { 0.f, 0.f };
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

void Enemy::update(float dt) {
    moveTimer += dt;

    // Прямоугольник следующей позиции
    sf::Vector2f nextPos = position + direction * speed * dt;
    sf::FloatRect nextBounds(nextPos, shape.getSize());

    // Проверка столкновения с блоками карты (через getGlobalBounds)
    bool collided = false;
    for (int y = 0; y < map.size(); ++y) {
        for (int x = 0; x < map[y].size(); ++x) {
            if (map[y][x] == '#' || map[y][x] == '*') {
                sf::FloatRect blockBounds(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
                if (nextBounds.intersects(blockBounds)) {
                    collided = true;
                    break;
                }
            }
        }
        if (collided) break;
    }

    // Если столкновение — смена направления
    if (collided) {
        direction = { 0.f, 0.f };
        moveTimer = moveDelay; // форсируем подбор направления
    }
    else {
        position = nextPos;
        shape.setPosition(position);
    }

    // Подбор нового направления
    if (moveTimer >= moveDelay && direction == sf::Vector2f{ 0.f, 0.f }) {
        moveTimer = 0.f;

        std::vector<sf::Vector2f> directions = {
            {1, 0}, {-1, 0}, {0, 1}, {0, -1}
        };
        std::random_shuffle(directions.begin(), directions.end());

        for (auto dir : directions) {
            sf::Vector2f testPos = position + dir * static_cast<float>(TILE_SIZE);
            sf::FloatRect testBounds(testPos, shape.getSize());

            bool testCollided = false;
            for (int y = 0; y < map.size(); ++y) {
                for (int x = 0; x < map[y].size(); ++x) {
                    if (map[y][x] == '#' || map[y][x] == '*') {
                        sf::FloatRect blockBounds(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
                        if (testBounds.intersects(blockBounds)) {
                            testCollided = true;
                            break;
                        }
                    }
                }
                if (testCollided) break;
            }

            if (!testCollided) {
                direction = dir;
                break;
            }
        }
    }
}





void Enemy::draw(sf::RenderWindow& window) {
    if (!isDead)
        window.draw(shape);
}

sf::Vector2i Enemy::getTilePosition() const {
    return sf::Vector2i(position.x / 60, position.y / 60);
}

sf::FloatRect Enemy::getBounds() const
{
    return shape.getGlobalBounds();
}
