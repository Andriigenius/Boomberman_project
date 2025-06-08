#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

extern sf::Texture IndestructibleBlockTexture;
extern const int TILE_SIZE;
extern std::vector<std::string> map;

void drawMap(sf::RenderWindow& window, sf::Texture& IndestructibleBlockTexture, sf::Texture& destructibleBlockTexture);
void generateRandomBlocks(float chance);