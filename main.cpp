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
#include "Bonus.h"
#include "AnimatedExplosion.h"


const int TILE_SIZE = 60;
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>



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

std::vector<std::string> map = baseMap;

std::vector<std::unique_ptr<Entity>> entities;
std::vector<Bonus> bonuses;
sf::Vector2i portalPos = { -1, -1 }; // Изначально портала нет
int destructibleBlocksLeft = 0;      // Счетчик оставшихся разрушаемых блоков


void drawPortal(sf::RenderWindow& window) {
    // Если портал не активен — ничего не рисуем
    if (portalPos.x == -1) return;

    // Создаём фиолетовый квадрат размером в 1 тайл
    sf::RectangleShape shape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    //shape.setFillColor(sf::Color::Magenta);

    sf::Texture portalTex;
    portalTex.loadFromFile("textures/portal.png");

    sf::Sprite portalSprite;
    portalSprite.setTexture(portalTex);
    shape.setPosition(portalPos.x * TILE_SIZE, portalPos.y * TILE_SIZE);
    portalSprite.setPosition(portalPos.x * TILE_SIZE, portalPos.y * TILE_SIZE);
    window.draw(portalSprite);
}

void destroyMap(sf::Vector2i center, Player& player, int lastBlock) {
    int radius = player.getFireRadius();
    std::vector<sf::Vector2i> explosionTiles; // Все клетки с визуальным взрывом
    std::vector<sf::Vector2i> destroyed;      // Клетки с уничтоженными разрушаемыми блоками

    static std::mt19937 ran(std::random_device{}());
    std::uniform_real_distribution<float> disss(0.f, 1.f);

    // Создаёт визуальный взрыв в указанной клетке
    auto addExplosion = [&](int x, int y) {
        sf::Vector2f pos(x * TILE_SIZE, y * TILE_SIZE);
        explosionTiles.push_back({ x, y });
        entities.push_back(std::make_unique<Explosion>(pos, TILE_SIZE));
        };

    // Проверяет клетку на разрушение, возвращает true если можно продолжать взрыв
    auto tryDestroy = [&](int x, int y) -> bool {
        if (map[y][x] == '#') return false;   // Не разрушать неразрушаемые блоки

        addExplosion(x, y);

        if (map[y][x] == '*') {
            map[y][x] = ' ';            // Удаляем блок с карты
            --destructibleBlocksLeft;   // Уменьшаем счётчик
            sf::Vector2i blockPos(x, y);

            static std::mt19937 rng(std::random_device{}());
            std::uniform_real_distribution<float> dist(0.f, 1.f);

            // Портал: если ещё не создан
            if (portalPos.x == -1) {
                float baseChance = 0.1f;

                bool isLast = (destructibleBlocksLeft == 0); // это последний блок

                if (dist(rng) < baseChance || isLast) {
                    portalPos = blockPos;
                    return false; // не генерировать бонус
                }
            }

            // Бонус: если это не портал
            if (portalPos != blockPos) {
                if (dist(rng) < 0.25f) {
                    BonusType type = static_cast<BonusType>(rand() % 2);
                    sf::Vector2f bonusPos(x * TILE_SIZE, y * TILE_SIZE);
                    bonuses.emplace_back(bonusPos, type);
                }
            }

            return true;
        }

        return true;
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

}

// Вызывается из main при выборе опции «Instructions»
void showInstructions(sf::RenderWindow& window) {
    // 1) Шрифт
    sf::Font font;
    if (!font.loadFromFile("fonts/Square.ttf")) {
        std::cerr << "Failed to load instructions font\n";
        return;
    }

    // 2) Текст строк инструкции
    std::vector<std::string> raw = {
        "HOW TO PLAY:",
        "   ",
        "- Use WASD to move -",
        "- Press SPACE to drop bombs -",
        "- Destroy enemies and blocks -",
        "- Find the portal to advance -"
    };
    std::vector<sf::Text> lines;
    float W = float(window.getSize().x), H = float(window.getSize().y);
    for (size_t i = 0; i < raw.size(); ++i) {
        sf::Text t(raw[i], font, 44);
        t.setFillColor(sf::Color::White);
        auto b = t.getLocalBounds();
        t.setOrigin(b.left + b.width / 2.f, b.top);
        t.setPosition(W / 2.f, H / 4.f + i * 40.f);
        lines.push_back(t);
    }

    // 3) Кнопка «Back»
    sf::Text back("Back", font, 36);
    back.setFillColor(sf::Color::Yellow);
    {
        auto b = back.getLocalBounds();
        back.setOrigin(b.left + b.width / 2.f, b.top);
        back.setPosition(W / 2.f, H * 3.f / 4.f);
    }

    // 4) Собственный mini-loop
    while (window.isOpen()) {
        sf::Event ev;
        while (window.pollEvent(ev)) {
            // закрыть приложение
            if (ev.type == sf::Event::Closed)
                window.close();

            // клик ЛКМ
            if (ev.type == sf::Event::MouseButtonReleased &&
                ev.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2f mp = window.mapPixelToCoords(
                    { ev.mouseButton.x, ev.mouseButton.y });
                if (back.getGlobalBounds().contains(mp)) {
                    // выход из инструкции - обратно в main()
                    return;
                }
            }

            // Esc или Enter тоже возвращают
            if (ev.type == sf::Event::KeyPressed) {
                if (ev.key.code == sf::Keyboard::Escape ||
                    ev.key.code == sf::Keyboard::Enter)
                {
                    return;
                }
            }
        }

        // 5) Отрисовка
        window.clear(sf::Color(30, 30, 30));
        for (auto& t : lines)  window.draw(t);
        window.draw(back);
        window.display();
    }
}



int main() {
    sf::Font square;
    if (!square.loadFromFile("fonts/Square.ttf")) {
        std::cerr << "Failed to load instructions font\n";
        return 0;
    }
    sf::Texture backgroundTexture;
    backgroundTexture.loadFromFile("textures/grass.png");
    backgroundTexture.setRepeated(true);

    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setTextureRect(sf::IntRect(0, 0, 1920, 1080));

    sf::Texture menuBackgroundTexture;
    menuBackgroundTexture.loadFromFile("textures/IndestructibleBlock.png");
    menuBackgroundTexture.setRepeated(true);

    sf::Sprite menuBackgroundSprite;
    menuBackgroundSprite.setTexture(menuBackgroundTexture);
    menuBackgroundSprite.setTextureRect(sf::IntRect(0, 0, 1920, 1080));


    sf::Texture IndestructibleBlockTexture;
    IndestructibleBlockTexture.loadFromFile("textures/IndestructibleBlock.png"); //textures/IndestructibleBlock.png

    sf::Texture destructibleBlockTexture;
    destructibleBlockTexture.loadFromFile("textures/destructibleBlock.png");

    sf::Texture bombTexture;
    bombTexture.loadFromFile("textures/Bomb.png");



    sf::Clock clock;
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "FullScreen", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    MainMenu menu(window.getSize().x, window.getSize().y, 0);
    bool inMenu = true;
    MainMenu gameOverMenu(window.getSize().x, window.getSize().y, 1);
    bool inGameOverMenu = false;

    map = baseMap;
    generateRandomBlocks(0.25f);
    destructibleBlocksLeft = 0;
    for (auto& row : map)
        for (char c : row)
            if (c == '*') ++destructibleBlocksLeft;

    Player player({ 2 * TILE_SIZE, 2 * TILE_SIZE }, { 36.f, 48.f });
    player.loadTextures();

    sf::Vector2i playerStartTile1{ 2, 2 }; // если игрок стартует на (2,2) в тайлах
    sf::Vector2i playerStartTile2{ 2, 3 }; // если игрок стартует на (2,2) в тайлах
    sf::Vector2i playerStartTile3{ 3, 2 }; // если игрок стартует на (2,2) в тайлах

    // 1) Собираем все пустые тайлы, кроме стартового
    std::vector<sf::Vector2i> freeTiles;
    for (int y = 0; y < map.size(); ++y) {
        for (int x = 0; x < map[y].size(); ++x) {
            if (map[y][x] == ' ') {
                sf::Vector2i t{ x, y };
                if (t != playerStartTile1 && t != playerStartTile2 && t != playerStartTile3)  // не берём стартовую клетку игрока
                    freeTiles.push_back(t);
            }
        }
    }

    // 2) Перемешиваем их случайно
    std::mt19937 rng{ std::random_device{}() };
    std::shuffle(freeTiles.begin(), freeTiles.end(), rng);

    // 3) Спавним ровно 6 или меньше, если свободных клеток меньше
    int spawnCount = std::min(6, (int)freeTiles.size());
    for (int i = 0; i < spawnCount; ++i) {
        sf::Vector2i tile = freeTiles[i];
        sf::Vector2f spawnPos(tile.x * TILE_SIZE,
            tile.y * TILE_SIZE);
        entities.push_back(
            std::make_unique<Enemy>(spawnPos, map)
        );
    }

    float bombCooldown = 2.0f;
    float bombCooldownTimer = 0.0f;

    sf::Font font;
    font.loadFromFile("fonts/Square_One_Bold.ttf");
    sf::Text loseText("GAME OVER", font, 100);
    loseText.setFillColor(sf::Color::Red);
    loseText.setPosition(500, 400);


    sf::Text gameOverScoreText, gameOverHighText;
    gameOverScoreText.setFont(square);
    gameOverScoreText.setCharacterSize(35);
    gameOverScoreText.setFillColor(sf::Color::White);
    gameOverScoreText.setPosition(window.getSize().x / 2.f - 130, window.getSize().y / 2.f - 140);

    gameOverHighText.setFont(square);
    gameOverHighText.setCharacterSize(35);
    gameOverHighText.setFillColor(sf::Color::Magenta);
    gameOverHighText.setPosition(window.getSize().x / 2.f - 150, window.getSize().y / 2.f - 200);


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

        for (auto& e : entities) e->update(dt);
        // … удаляем взрывы, собираем бонусы и т.п …

        // здесь вставляем проверку столкновений:
        for (auto& ent : entities) {
            if (auto* enemy = dynamic_cast<Enemy*>(ent.get())) {
                if (!enemy->isDead && enemy->getBounds().intersects(player.getBounds())) {
                    player.isDead = true;
                    /*inMenu = false;
                    inGameOverMenu = true;*/
                    break;
                }
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
                    // Рестарт
                    inGameOverMenu = false;
                    inMenu = true; // возвращаемся в главное меню
                    score = 0;
                    entities.clear();
                    portalPos = { -1, -1 };
                    map = baseMap;
                    generateRandomBlocks(0.25f);
                    destructibleBlocksLeft = 0;
                    for (auto& row : map)
                        for (char c : row)
                            if (c == '*') ++destructibleBlocksLeft;
                    player.setPos({ 2 * TILE_SIZE, 2 * TILE_SIZE });


                    {// 1) Собираем все пустые тайлы, кроме стартового
                        std::vector<sf::Vector2i> freeTiles;
                        for (int y = 0; y < map.size(); ++y) {
                            for (int x = 0; x < map[y].size(); ++x) {
                                if (map[y][x] == ' ') {
                                    sf::Vector2i t{ x, y };
                                    if (t != playerStartTile1 && t != playerStartTile2 && t != playerStartTile3)  // не берём стартовую клетку игрока
                                        freeTiles.push_back(t);
                                }
                            }
                        }

                        // 2) Перемешиваем их случайно
                        std::mt19937 rng{ std::random_device{}() };
                        std::shuffle(freeTiles.begin(), freeTiles.end(), rng);

                        // 3) Спавним ровно 6 или меньше, если свободных клеток меньше
                        int spawnCount = std::min(6, (int)freeTiles.size());
                        for (int i = 0; i < spawnCount; ++i) {
                            sf::Vector2i tile = freeTiles[i];
                            sf::Vector2f spawnPos(tile.x * TILE_SIZE,
                                tile.y * TILE_SIZE);
                            entities.push_back(
                                std::make_unique<Enemy>(spawnPos, map)
                            );
                        }
                    } //появление врагов


                }
                else if (choice == 1) {
                    window.close();
                }
            }

            continue; // пропускаем остальной игровой код
        }


        for (auto it = bonuses.begin(); it != bonuses.end();) {
            if (player.getBounds().intersects(it->getBounds())) {
                switch (it->getType()) {
                case BonusType::FirePower:
                    player.setFireRadius(player.getFireRadius() + 1);
                    break;
                case BonusType::SpeedUp:
                    player.setSpeed(player.getSpeed() + 25.f);
                    break;
                }
                it = bonuses.erase(it);
            }
            else {
                ++it;
            }
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
                    // здесь должен начаться мой игровой цикл
                }
                else if (option == 1) {
                    showInstructions(window);
                }
                else if (option == 2)
                {
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
                    }, bombTexture
                ));
                bombCooldownTimer = 0.f;
            }


            if (portalPos.x != -1 && player.getTilePosition() == portalPos) {
                portalPos = { -1, -1 };
                bonuses.clear();
                entities.clear();
                map = baseMap;
                generateRandomBlocks(0.25f);

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
                player.setFireRadius(1);
                player.setSpeed(150.f);

                bonuses.clear();

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
            scoreText.setFont(square);
            scoreText.setCharacterSize(34);
            scoreText.setFillColor(sf::Color::White);
            scoreText.setPosition(880, 8);

            // В игровом режиме, перед window.display():
            scoreText.setString("Score: " + std::to_string(score));

            window.draw(backgroundSprite);
            drawMap(window, IndestructibleBlockTexture, destructibleBlockTexture);
            drawPortal(window);
            player.draw(window);
            for (auto& e : entities)
                e->draw(window);
            window.draw(scoreText);
            for (auto& bonus : bonuses)
                bonus.draw(window);
            window.display();
        }
    }
    return 0;
}