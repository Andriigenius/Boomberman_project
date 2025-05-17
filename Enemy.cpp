#include "Enemy.h"
#include <cstdlib>
#include <ctime>
extern const int TILE_SIZE;

Enemy::Enemy(sf::Vector2f startPos, const std::vector<std::string>& mapRef)
    : position(startPos), map(mapRef), speed(60.f), moveTimer(0.f), moveDelay(1.f) {
    shape.setSize({ 55.f, 55.f });
    shape.setFillColor(sf::Color::Red);
    shape.setPosition(position);
    direction = { 0.f, 0.f };
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

void Enemy::update(float dt) {
    moveTimer += dt;

    if (moveTimer >= moveDelay) {
        moveTimer = 0.f;

        std::vector<sf::Vector2f> directions = {
            {1, 0}, {-1, 0}, {0, 1}, {0, -1}
        };

        std::random_shuffle(directions.begin(), directions.end());

        for (auto dir : directions) {
            sf::Vector2f testPos = position + dir * static_cast<float>(TILE_SIZE);
            int x = static_cast<int>(testPos.x / TILE_SIZE);
            int y = static_cast<int>(testPos.y / TILE_SIZE);

            if (x >= 0 && x < map[0].size() && y >= 0 && y < map.size()) {
                if (map[y][x] == ' ') {
                    direction = dir;
                    break;
                }
            }
        }
    }

    // Проверка перед каждым движением
    sf::Vector2f nextPos = position + direction * speed * dt;
    int nextX = static_cast<int>(nextPos.x / TILE_SIZE);
    int nextY = static_cast<int>(nextPos.y / TILE_SIZE);

    if (nextX >= 0 && nextX < map[0].size() &&
        nextY >= 0 && nextY < map.size() &&
        map[nextY][nextX] == ' ') {
        position = nextPos;
        shape.setPosition(position);
    }
}


void Enemy::draw(sf::RenderWindow& window) {
    if (!isDead)
        window.draw(shape);
}

sf::Vector2i Enemy::getTilePosition() const {
    return sf::Vector2i(position.x / 60, position.y / 60);
}
