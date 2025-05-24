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

void destroyMap(sf::Vector2i center, Player& player) {
    int radius = 1;
    std::vector<sf::Vector2i> explosionTiles;

    auto addExplosion = [&](int x, int y) {
        sf::Vector2f pos(x * TILE_SIZE, y * TILE_SIZE);
        explosionTiles.push_back({ x, y });
        entities.push_back(std::make_unique<Explosion>(pos, TILE_SIZE));
        };
    // Центр
    addExplosion(center.x, center.y);

    // Влево
    for (int dx = 1; dx <= radius; ++dx) {
        int x = center.x - dx;
        int y = center.y;
        if (map[y][x] == '#') break;
        addExplosion(x, y);
        if (map[y][x] == '*') {
            map[y][x] = ' ';
            break;
        }
    }

    // Вправо
    for (int dx = 1; dx <= radius; ++dx) {
        int x = center.x + dx;
        int y = center.y;
        if (map[y][x] == '#') break;
        addExplosion(x, y);
        if (map[y][x] == '*') {
            map[y][x] = ' ';
            break;
        }
    }


    // ⬆ Вверх
    for (int dy = 1; dy <= radius; ++dy) {
        int x = center.x;
        int y = center.y - dy;
        if (map[y][x] == '#') break;
        addExplosion(x, y);
        if (map[y][x] == '*') {
            map[y][x] = ' ';
            break;
        }
    }

    // ⬇ Вниз
    for (int dy = 1; dy <= radius; ++dy) {
        int x = center.x;
        int y = center.y + dy;
        if (map[y][x] == '#') break;
        addExplosion(x, y);
        if (map[y][x] == '*') {
            map[y][x] = ' ';
            break;
        }
    }

    for (auto& tile : explosionTiles) {
        for (auto& e : entities) {
            if (auto* enemy = dynamic_cast<Enemy*>(e.get())) {
                sf::FloatRect enemyRect = enemy->getBounds();
                sf::FloatRect tileRect(tile.x * TILE_SIZE, tile.y * TILE_SIZE, TILE_SIZE, TILE_SIZE);

                if (!enemy->isDead && enemyRect.intersects(tileRect))
                    enemy->isDead = true;

            }
        }

        sf::FloatRect tileRect(tile.x * TILE_SIZE, tile.y * TILE_SIZE, TILE_SIZE, TILE_SIZE);

        if (player.getBounds().intersects(tileRect))
            player.isDead = true;
    }
}






int main() {

    sf::Clock clock;

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "FullScreen", sf::Style::Fullscreen); // 13x7 tiles
    window.setFramerateLimit(60);

    generateRandomBlocks(0.23f); // рандомная догенирация * (разрушаемых блоков) в вектор map на место " ". 

    Player player({ 2 * TILE_SIZE, 2 * TILE_SIZE }, { 40.f, 40.f });// определение позиции стартовой для игрока

    for (int i = 0; i < 6; i++) {
        sf::Vector2f enemySpawnPos;
        bool found = false;

        while (!found) {
            int x = rand() % map[0].size();
            int y = rand() % map.size();

            if (map[y][x] == ' ') {
                enemySpawnPos = sf::Vector2f(x * TILE_SIZE, y * TILE_SIZE);
                found = true;
            }
        }

        entities.push_back(std::make_unique<Enemy>(enemySpawnPos, map));
    }

    float bombCooldown = 2.0f;
    float bombCooldownTimer = 0.0f;


    sf::Font font;
    font.loadFromFile("fonts/ARIAL.TTF");
    sf::Text loseText("GAME OVER", font, 100);
    loseText.setFillColor(sf::Color::Red);
    loseText.setPosition(500, 400);

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f; // максимум 100 мс


        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed || ( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape )  )
                window.close();

            bombCooldownTimer += dt;
            
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && bombCooldownTimer >= bombCooldown) {
                sf::Vector2f center = {
                    player.getBounds().left + player.getBounds().width / 2.f,
                    player.getBounds().top + player.getBounds().height / 2.f
                };

                    sf::Vector2i tile = sf::Vector2i(
                        static_cast<int>(center.x) / TILE_SIZE,
                        static_cast<int>(center.y) / TILE_SIZE
                    );

                    sf::Vector2f bombPos(tile.x * TILE_SIZE, tile.y * TILE_SIZE);

                

                entities.push_back(std::make_unique<Bomb>(
                    bombPos, 2.0f, TILE_SIZE,
                    [&player](sf::Vector2i center) {
                        destroyMap(center, player);
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
        if (player.isDead)
            window.draw(loseText);
        window.display();
    }

    return 0;
}