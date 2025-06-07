#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include <random>

#include "MapDrawer.h"
#include "Player.h"
#include "Bomb.h"
#include "Enemy.h"
#include "Explosion.h"
#include "MainMenu.h"
#include "ScoreManager.h"

const int TILE_SIZE = 60;

const std::vector<std::string> baseMap = {
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
sf::Vector2i portalPos = { -1, -1 }; // Изначально портала нет
int destructibleBlocksLeft = 0;      // Счетчик оставшихся разрушаемых блоков

void drawPortal(sf::RenderWindow& window) {
    // Если портал не активен — ничего не рисуем
    if (portalPos.x == -1) return;

    // Создаём фиолетовый квадрат размером в 1 тайл
    sf::RectangleShape shape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    shape.setFillColor(sf::Color::Magenta);
    shape.setPosition(portalPos.x * TILE_SIZE, portalPos.y * TILE_SIZE);
    window.draw(shape);
}

void destroyMap(sf::Vector2i center, Player& player, int lastBlock) {
    int radius = 1;
    std::vector<sf::Vector2i> explosionTiles; // Все клетки с визуальным взрывом
    std::vector<sf::Vector2i> destroyed;      // Клетки с уничтоженными разрушаемыми блоками

    // Создаёт визуальный взрыв в указанной клетке
    auto addExplosion = [&](int x, int y) {
        sf::Vector2f pos(x * TILE_SIZE, y * TILE_SIZE);
        explosionTiles.push_back({ x, y });
        entities.push_back(std::make_unique<Explosion>(pos, TILE_SIZE));
        };

    // Проверяет клетку на разрушение, возвращает true если можно продолжать взрыв
    auto tryDestroy = [&](int x, int y) {
        if (map[y][x] == '#') return false; // Не разрушать неразрушаемые блоки
        addExplosion(x, y);
        if (map[y][x] == '*') {
            map[y][x] = ' ';                // Удаляем блок с карты
            --destructibleBlocksLeft;       // Уменьшаем счётчик
            destroyed.push_back({ x, y });  // Добавляем в список разрушенных
            return false;                   // Остановить распространение
        }
        return true; // Пустая клетка — продолжаем
        };

    // Центр взрыва
    addExplosion(center.x, center.y);
    tryDestroy(center.x, center.y);

    // Взрыв влево
    for (int dx = 1; dx <= radius; ++dx) {
        if (!tryDestroy(center.x - dx, center.y)) break;
    }
    // Взрыв вправо
    for (int dx = 1; dx <= radius; ++dx) {
        if (!tryDestroy(center.x + dx, center.y)) break;
    }
    // Взрыв вверх
    for (int dy = 1; dy <= radius; ++dy) {
        if (!tryDestroy(center.x, center.y - dy)) break;
    }
    // Взрыв вниз
    for (int dy = 1; dy <= radius; ++dy) {
        if (!tryDestroy(center.x, center.y + dy)) break;
    }

    // Проверка урона по врагам и игроку
    for (auto& tile : explosionTiles) {
        sf::FloatRect tileRect(tile.x * TILE_SIZE, tile.y * TILE_SIZE, TILE_SIZE, TILE_SIZE);

        // Урон врагам
        for (auto& e : entities) {
            if (auto* enemy = dynamic_cast<Enemy*>(e.get())) {
                if (!enemy->isDead && enemy->getBounds().intersects(tileRect))
                    enemy->isDead = true;
            }
        }

        // Урон игроку
        if (player.getBounds().intersects(tileRect))
            player.isDead = true;
    }

    // Генерация портала в разрушенном блоке с шансом
    if (!destroyed.empty() && portalPos.x == -1) {
        static std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> dist(0.f, 1.f);

        float baseChance = 0.1f; // Начальный шанс 5%
        float chance = baseChance;
        bool isLast = false;

        for (size_t i = 0; i < destroyed.size(); ++i) {
            if (lastBlock == 1)
            {
                isLast = true;
            }
            if (dist(rng) <= chance || isLast) {
                portalPos = destroyed[i]; // Сохраняем координату портала
                break;
            }
        }
    }
}

int main() {
    sf::Clock clock;
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "FullScreen", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    MainMenu menu(window.getSize().x, window.getSize().y, 0);
    bool inMenu = true;
    MainMenu gameOverMenu(window.getSize().x, window.getSize().y, 1);
    bool inGameOverMenu = false;

    map = baseMap;
    generateRandomBlocks(0.3333f);
    destructibleBlocksLeft = 0;
    for (auto& row : map)
        for (char c : row)
            if (c == '*') ++destructibleBlocksLeft;

    Player player({ 2 * TILE_SIZE, 2 * TILE_SIZE }, { 56.f, 56.f });

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


    sf::Text gameOverScoreText, gameOverHighText;
    gameOverScoreText.setFont(font);
    gameOverScoreText.setCharacterSize(30);
    gameOverScoreText.setFillColor(sf::Color::White);
    gameOverScoreText.setPosition(window.getSize().x / 2.f - 100, window.getSize().y / 2.f - 100);

    gameOverHighText.setFont(font);
    gameOverHighText.setCharacterSize(30);
    gameOverHighText.setFillColor(sf::Color::Magenta);
    gameOverHighText.setPosition(window.getSize().x / 2.f - 100, window.getSize().y / 2.f - 60);


    int score = 0;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
            {
                window.close();
            }

            if (inMenu) {
                menu.handleEvent(event, window);
            }
            
            if (inGameOverMenu) {
                gameOverMenu.handleEvent(event, window);
            }
        }
        
        if (inGameOverMenu) {
            gameOverMenu.handleEvent(event, window);

            gameOverMenu.update(window);
            window.clear(sf::Color(30, 0, 0)); // темный красный фон, например
            gameOverMenu.draw(window);
            window.draw(gameOverScoreText);
            window.draw(gameOverHighText);
            window.display();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) ||
                sf::Mouse::isButtonPressed(sf::Mouse::Left)) {

                int choice = gameOverMenu.getSelectedOption();
                if (choice == 0) {
                    // ⭯ Restart
                    inGameOverMenu = false;
                    inMenu = true; // возвращаемся в главное меню
                    score = 0;

                    entities.clear();
                    portalPos = { -1, -1 };
                    map = baseMap;
                    generateRandomBlocks(0.3333f);
                    destructibleBlocksLeft = 0;
                    for (auto& row : map)
                        for (char c : row)
                            if (c == '*') ++destructibleBlocksLeft;
                    player.setPos({ 2 * TILE_SIZE, 2 * TILE_SIZE });
                }
                else if (choice == 1) {
                    window.close();
                }
            }

            continue; // пропускаем остальной игровой код
        }

        if (inMenu) {
            menu.update(window);
        }


        window.clear(); // Тёмный фон
        if (inMenu) {
            menu.draw(window);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) ||
                sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                int option = menu.getSelectedOption();
                if (option == 0) {
                    // Start game
                    inMenu = false;
                    // здесь должен начаться твой игровой цикл
                }
                else if (option == 1) {
                    window.close();
                }
            }
            window.display();
            continue;
        }
        else
        {
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
                        destroyMap(center, player, destructibleBlocksLeft);
                    }
                ));

                bombCooldownTimer = 0.f;
            }

            if (portalPos.x != -1 && player.getTilePosition() == portalPos) {
                portalPos = { -1, -1 };
                entities.clear();
                map = baseMap;
                generateRandomBlocks(0.3333f);

                destructibleBlocksLeft = 0;
                for (auto& row : map)
                    for (char c : row)
                        if (c == '*') ++destructibleBlocksLeft;

                player.setPos({ 2 * TILE_SIZE, 2 * TILE_SIZE });

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
            }
            if (player.isDead) {
                ScoreManager::saveScore(score);
                gameOverScoreText.setString("Your Score: " + std::to_string(score));
                gameOverHighText.setString("High Score: " + std::to_string(ScoreManager::loadHighScore()));
                inGameOverMenu = true;
                player.isDead = false; // сбрасываем, чтобы не зациклилось
                continue;              // пропускаем кадр
            }

            player.Moveset();
            player.update(dt);
            player.handleCollision(map);

            for (auto& e : entities)
                e->update(dt);

            entities.erase(std::remove_if(entities.begin(), entities.end(),
                [&score](const std::unique_ptr<Entity>& e) {
                    if (auto* enemy = dynamic_cast<Enemy*>(e.get())) {
                        if (enemy->isDead) {
                            score += 100; // +100 очков за врага
                            return true;
                        }
                    }
                    if (auto* explosion = dynamic_cast<Explosion*>(e.get()))
                        return explosion->isFinished();
                    return false;
                }),
                entities.end());

            sf::Text scoreText;
            scoreText.setFont(font);
            scoreText.setCharacterSize(24);
            scoreText.setFillColor(sf::Color::White);
            scoreText.setPosition(20, 10);

            // В игровом режиме, перед window.display():
            scoreText.setString("Score: " + std::to_string(score));

            drawMap(window);
            drawPortal(window);
            player.draw(window);
            for (auto& e : entities)
                e->draw(window);
            drawMap(window);
            window.draw(scoreText);
            window.display();
        }
    }
    return 0;
}
