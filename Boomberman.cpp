#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "MapDrawer.h"
#include "Player.h"
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


int main() {

    sf::Clock clock;

    sf::VideoMode dekstop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(dekstop, "FullScreen", sf::Style::Fullscreen); // 13x7 tiles
    window.setFramerateLimit(60);
    generateRandomBlocks(0.3f); // рандомная догенирация * (разрушаемых блоков) в вектор map на место " ". 
    Player player({ 2 * TILE_SIZE, 2 * TILE_SIZE }, { 40.f, 40.f });// определение позиции стартовой для игрока


    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f; // максимум 100 мс


        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed || ( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape )  )
                window.close();

        player.Moveset();     // обработка ввода
        player.update(dt);           // общая логика
        player.handleCollision(map);

        window.clear();
        drawMap(window);             // карта
        player.draw(window);         // игрок
        drawMap(window);
        window.display();
    }

    return 0;
}