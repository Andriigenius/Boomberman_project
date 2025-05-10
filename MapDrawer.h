#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

extern const int TILE_SIZE;
extern std::vector<std::string> map;

void drawMap(sf::RenderWindow& window);
void generateRandomBlocks(float chance);