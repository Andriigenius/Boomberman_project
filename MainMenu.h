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
    sf::Font BoldFont;
    sf::Font UsualFont;
    sf::Text title;
    sf::Text hint;
    std::vector<sf::Text> options;
    int selectedIndex;

    int screenType;                         
    std::vector<sf::Text> instructions;
    sf::Texture bgTexture;
    sf::Sprite  bgSprite;

    void updateSelection(int newIndex);
};
