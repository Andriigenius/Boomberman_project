#include "MapDrawer.h"
#include <iostream>
#include <random>


void drawMap(sf::RenderWindow& window) {
    sf::RectangleShape tileShape(sf::Vector2f(TILE_SIZE, TILE_SIZE));

    for (int y = 0; y < map.size(); ++y) {
        for (int x = 0; x < map[y].size(); ++x) {
            char tile = map[y][x];

            if (tile == '#') {
                tileShape.setFillColor(sf::Color::Blue);
            }
            else if (tile == '*') {
                tileShape.setFillColor(sf::Color::Cyan);
            }
            else {
                continue;
            }

            tileShape.setPosition(x * TILE_SIZE, y * TILE_SIZE);
            window.draw(tileShape);
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