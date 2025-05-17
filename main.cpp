#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <algorithm> // для std::remove_if


#include "MapDrawer.h"
#include "Player.h"
#include "Bomb.h"
#include "Enemy.h"
#include "Explosion.h"
const int TILE_SIZE = 60;

std::vector<std::string> map = {
    "################################",
    "################################",
    "#                             ##",
    "# # # # # # # # # # # # # # # ##",
    "#                             ##",
    "# # # # # # # # # # # # # # # ##",
    "#                             ##",
    "# # # # # # # # # # # # # # # ##",
    "#                             ##",
    "# # # # # # # # # # # # # # # ##",
    "#                             ##",
    "# # # # # # # # # # # # # # # ##",
    "#                             ##",
    "# # # # # # # # # # # # # # # ##",
    "#                             ##",
    "# # # # # # # # # # # # # # # ##",
    "#                             ##",
    "################################"
};

std::vector<std::unique_ptr<Entity>> entities;

void destroyMap(sf::Vector2i center) {
    int radius = 1;
    std::vector<sf::Vector2i> explosionTiles;

    auto addExplosion = [&](int x, int y) {
        sf::Vector2f pos(x * TILE_SIZE, y * TILE_SIZE);
        explosionTiles.push_back({ x, y });
        entities.push_back(std::make_unique<Explosion>(pos, TILE_SIZE));
        };

    addExplosion(center.x, center.y);

    for (int dx = -radius; dx <= radius; ++dx) {
        if (dx == 0) continue;
        int x = center.x + dx;
        int y = center.y;
        if (map[y][x] == '#') break;
        addExplosion(x, y);
        if (map[y][x] == '*') {
            map[y][x] = ' ';
            break;
        }
    }

    for (int dy = -radius; dy <= radius; ++dy) {
        if (dy == 0) continue;
        int x = center.x;
        int y = center.y + dy;
        if (map[y][x] == '#') break;
        addExplosion(x, y);
        if (map[y][x] == '*') {
            map[y][x] = ' ';
            break;
        }
    }

    // Убиваем врагов во ВСЕХ клетках взрыва
    for (auto& e : entities) {
        if (auto* enemy = dynamic_cast<Enemy*>(e.get())) {
            if (enemy->isDead) continue;

            for (auto& tile : explosionTiles) {
                if (enemy->getTilePosition() == tile) {
                    enemy->isDead = true;
                    break;
                }
            }
        }
    }
}






int main() {

    sf::Clock clock;

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "FullScreen", sf::Style::Fullscreen); // 13x7 tiles
    window.setFramerateLimit(60);

    generateRandomBlocks(0.3f); // рандомная догенирация * (разрушаемых блоков) в вектор map на место " ". 

    Player player({ 2 * TILE_SIZE, 2 * TILE_SIZE }, { 40.f, 40.f });// определение позиции стартовой для игрока


    entities.push_back(std::make_unique<Enemy>(sf::Vector2f(5 * TILE_SIZE, 5 * TILE_SIZE), map));

    float bombCooldown = 2.0f;
    float bombCooldownTimer = 0.0f;


    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f; // максимум 100 мс


        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed || ( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape )  )
                window.close();

            bombCooldownTimer += dt;
            
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && bombCooldownTimer >= bombCooldown) {
                sf::Vector2f bombPos = {
                    static_cast<float>(player.getTilePosition().x * TILE_SIZE),
                    static_cast<float>(player.getTilePosition().y * TILE_SIZE)
                };

                entities.push_back(std::make_unique<Bomb>(
                    bombPos, 2.0f, TILE_SIZE,
                    [](sf::Vector2i center) {
                        destroyMap(center);
                    }
                ));

                bombCooldownTimer = 0.f; // сброс кулдауна
            }




        

        player.Moveset();     // обработка ввода
        player.update(dt);           // общая логика
        player.handleCollision(map);
        for (auto& e : entities)
            e->update(dt);

        entities.erase(std::remove_if(entities.begin(), entities.end(),
            [](const std::unique_ptr<Entity>& e) {
                if (auto* enemy = dynamic_cast<Enemy*>(e.get())) return enemy->isDead;
                if (auto* explosion = dynamic_cast<Explosion*>(e.get())) return explosion->isFinished();
                return false;
            }),
            entities.end());



        window.clear();
        drawMap(window);             // карта
        player.draw(window);         // игрок
        for (auto& e : entities)
            e->draw(window);
        drawMap(window);
        window.display();
    }

    return 0;
}