#include "MapDrawer.h"
#include <iostream>
#include <random>

void drawMap(sf::RenderWindow& window, sf::Texture& IndestructibleBlockTexture, sf::Texture& destructibleBlockTexture) {
    sf::RectangleShape tileShape(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    sf::Sprite NotDestileSprite;
    NotDestileSprite.setTexture(IndestructibleBlockTexture);
    sf::Sprite DestileSprite;
    DestileSprite.setTexture(destructibleBlockTexture);

    for (int y = 0; y < map.size(); ++y) {
        for (int x = 0; x < map[y].size(); ++x) {
            char tile = map[y][x];

            if (tile == ' ') { continue; }
            else if (tile == '#') {
                NotDestileSprite.setPosition(x * TILE_SIZE, y * TILE_SIZE);
                window.draw(NotDestileSprite); // рисуем неразрушаемый блок с текстурой
            }
            else if (tile == '*') {
                DestileSprite.setPosition(x * TILE_SIZE, y * TILE_SIZE);
                window.draw(DestileSprite); // рисуем разрушаемый блок цветом
            }
        }
    }
}


void generateRandomBlocks(float chance) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    int counterOfBlocks = 0;
    for (int y = 0; y < map.size(); ++y) {
        for (int x = 0; x < map[y].size(); ++x) {
            if ((y == 2 && x == 1) || (y == 3 && x == 1) || (y == 2 && x == 2)) { continue; }
            if (map[y][x] == ' ') {
                if (dis(gen) < chance) {
                    map[y][x] = '*'; // ставим разрушаемый блок
                }
            }
        }
    }
}