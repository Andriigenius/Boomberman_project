#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class MainMenu {
public:
    MainMenu(float width, float height, int SEscreen);

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update(const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
    int getSelectedOption() const;

private:
    sf::Font font;
    sf::Text title;
    sf::Text hint;
    std::vector<sf::Text> options;
    int selectedIndex;

    void updateSelection(int newIndex);
};
