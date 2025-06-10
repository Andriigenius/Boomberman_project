#include "Enemy.h"
#include <cstdlib>
#include <ctime>
extern const int TILE_SIZE;

Enemy::Enemy(sf::Vector2f startPos, const std::vector<std::string>& mapRef)
    : position(startPos), map(mapRef), speed(60.f), moveTimer(0.f), moveDelay(1.f) 
{
    // загрузка текстур (поменяй пути под свои файлы)
    texturesDown[0].loadFromFile("textures/enemyS1.png");
    texturesDown[1].loadFromFile("textures/enemyS2.png");

    texturesUp[0].loadFromFile("textures/enemyB1.png");
    texturesUp[1].loadFromFile("textures/enemyB2.png");

    texturesLeft[0].loadFromFile("textures/enemyL1.png");
    texturesLeft[1].loadFromFile("textures/enemyL2.png");

    texturesRight[0].loadFromFile("textures/enemyR1.png");
    texturesRight[1].loadFromFile("textures/enemyR2.png");

    // выставляем первый кадр и позицию
    sprite.setTexture(texturesDown[0]);
    sprite.setPosition(position);

    shape.setSize({ 60.f, 60.f });
    shape.setFillColor(sf::Color::Red);
    shape.setPosition(position);
    direction = { 0.f, 0.f };
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

void Enemy::update(float dt) {
    moveTimer += dt;

    // 1) если столкнулся, сброс направления и форсировать выбор нового
    sf::Vector2f nextPos = position + direction * speed * dt;
    sf::FloatRect nextBounds(nextPos, sprite.getGlobalBounds().getSize());
    bool collided = false;

    for (int y = 0; y < map.size() && !collided; ++y)
        for (int x = 0; x < map[y].size() && !collided; ++x)
            if (map[y][x] == '#' || map[y][x] == '*') {
                sf::FloatRect blockBounds(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
                if (nextBounds.intersects(blockBounds))
                    collided = true;
            }

    if (collided) {
        direction = { 0,0 };
        moveTimer = moveDelay; // чтобы сразу сменить направление
        isMoving = false;
    }
    else {
        position = nextPos;
        sprite.setPosition(position);
        shape.setPosition(position);
        isMoving = (direction.x != 0.f || direction.y != 0.f);
    }

    // 2) выбор нового направления
    if (moveTimer >= moveDelay && direction == sf::Vector2f{ 0,0 }) {
        moveTimer = 0;
        std::vector<sf::Vector2f> dirs = { {1,0},{-1,0},{0,1},{0,-1} };
        std::random_shuffle(dirs.begin(), dirs.end());
        for (auto d : dirs) {
            sf::Vector2f testPos = position + d * static_cast<float>(TILE_SIZE);
            sf::FloatRect testB(testPos, sprite.getGlobalBounds().getSize());
            bool bad = false;
            for (int y = 0; y < map.size() && !bad; ++y)
                for (int x = 0; x < map[y].size() && !bad; ++x)
                    if ((map[y][x] == '#' || map[y][x] == '*') && testB.intersects(
                        sf::FloatRect(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE)))
                        bad = true;
            if (!bad) { direction = d; break; }
        }
    }

    // 3) анимация: кадр и направление
    if (isMoving) {
        frameTimer += dt;
        if (frameTimer >= frameDuration) {
            frameTimer = 0;
            frame = (frame + 1) % 2;
        }
        // определяем animDir по direction
        if (direction.x > 0)      animDir = EDir::Right;
        else if (direction.x < 0) animDir = EDir::Left;
        else if (direction.y > 0) animDir = EDir::Down;
        else if (direction.y < 0) animDir = EDir::Up;
    }
    else {
        frame = 0; // стоим — первый кадр
    }

    // применяем текстуру
    switch (animDir) {
    case EDir::Down:  sprite.setTexture(texturesDown[frame]);  break;
    case EDir::Up:    sprite.setTexture(texturesUp[frame]);    break;
    case EDir::Left:  sprite.setTexture(texturesLeft[frame]);  break;
    case EDir::Right: sprite.setTexture(texturesRight[frame]); break;
    }
}

void Enemy::draw(sf::RenderWindow& window) {
    if (!isDead)
        window.draw(sprite);
}

sf::Vector2i Enemy::getTilePosition() const {
    return sf::Vector2i(position.x / 60, position.y / 60);
}

sf::FloatRect Enemy::getBounds() const
{
    return shape.getGlobalBounds();
}

/*void Enemy::update(float dt) {
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
}*/