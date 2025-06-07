#include "MainMenu.h"
#include <iostream>

MainMenu::MainMenu(float width, float height, int SEscreen) : selectedIndex(0) {
    if (!font.loadFromFile("fonts/ARIAL.TTF")) {
        std::cerr << "Ошибка загрузки шрифта!" << std::endl;
    }

    // Заголовок
    title.setFont(font);
    if (SEscreen == 0) {
        title.setString("BOMBERMAN");
    }
    if (SEscreen == 1) {
        title.setString("GAME OVER");
    }
    title.setCharacterSize(64);
    title.setFillColor(sf::Color::White);
    title.setPosition(width / 2.f - 180, height / 6.f);

    // Подсказка
    hint.setFont(font);
    hint.setString("Use Arrow Keys or Mouse | Enter or Click");
    hint.setCharacterSize(20);
    hint.setFillColor(sf::Color(200, 200, 200));
    hint.setPosition(width / 2.f - 160, height - 50);

    // Кнопки меню
    std::vector<std::string> labels;
    if (SEscreen == 0){ labels = { "Start Game", "Exit" }; }
    if (SEscreen == 1){ labels = { "Restart", "Exit" }; }

    for (size_t i = 0; i < labels.size(); ++i) {
        sf::Text text;
        text.setFont(font);
        text.setString(labels[i]);
        text.setCharacterSize(40);
        text.setPosition(width / 2.f - 100, height / 2.f + i * 70);
        text.setFillColor(i == 0 ? sf::Color::Yellow : sf::Color::White);
        options.push_back(text);
    }
}

void MainMenu::updateSelection(int newIndex) {
    if (newIndex >= 0 && newIndex < static_cast<int>(options.size())) {
        for (auto& opt : options)
            opt.setFillColor(sf::Color::White);
        options[newIndex].setFillColor(sf::Color::Yellow);
        selectedIndex = newIndex;
    }
}

void MainMenu::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Up)
            updateSelection((selectedIndex + options.size() - 1) % options.size());
        else if (event.key.code == sf::Keyboard::Down)
            updateSelection((selectedIndex + 1) % options.size());
    }

    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        for (size_t i = 0; i < options.size(); ++i) {
            if (options[i].getGlobalBounds().contains(mousePos)) {
                updateSelection(i);
            }
        }
    }

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        for (size_t i = 0; i < options.size(); ++i) {
            if (options[i].getGlobalBounds().contains(mousePos)) {
                updateSelection(i);
            }
        }
    }
}

void MainMenu::update(const sf::RenderWindow& window) {
    // Можно добавить анимации или фон
}

void MainMenu::draw(sf::RenderWindow& window) {
    window.draw(title);
    for (const auto& option : options)
        window.draw(option);
    window.draw(hint);
}

int MainMenu::getSelectedOption() const {
    return selectedIndex;
}
