#include "MainMenu.h"
#include <iostream>

MainMenu::MainMenu(float width, float height, int SEscreen) : selectedIndex(0) {
    if (!BoldFont.loadFromFile("fonts/Square_One_Bold.ttf")) {
        std::cerr << "Ошибка загрузки шрифта!" << std::endl;
    }

    if (!UsualFont.loadFromFile("fonts/Square.ttf")) {
        std::cerr << "Ошибка загрузки шрифта!" << std::endl;
    }

    if (!bgTexture.loadFromFile("textures/grass.png")) {
        std::cerr << "Ошибка загрузки menu_background.png\n";
    }

    bgTexture.setRepeated(true);

    bgSprite.setTexture(bgTexture);
    bgSprite.setTextureRect(sf::IntRect(0, 0, 1920, 1080));
    // масштабируем, чтобы он занял весь экран

    // Заголовок
    title.setFont(BoldFont);
    if (SEscreen == 0) {
        title.setString("BOMBERMAN");
    }
    if (SEscreen == 1) {
        title.setString("GAME OVER");
    }
    // После того, как мы задали шрифт, строку и размер:
    title.setCharacterSize(128);
    title.setFillColor(sf::Color::White);
    title.setString(SEscreen == 0 ? "BOMBERMAN" : "GAME OVER");

    // 1. Считаем «границы» текста
    sf::FloatRect bounds = title.getLocalBounds();

    // 2. Ставим точку отсчёта (origin) в центр по горизонтали и «выровненный» по верху:
    //    - bounds.left обычно небольшой отрицательный отступ шрифта
    //    - bounds.top  смещение по вертикали (чтобы строка рисовалась ровно от верхней грани)
    title.setOrigin(bounds.left + bounds.width / 2.f, bounds.top);

    // 3. Центрируем по середине окна, чуть ниже верхней трети
    float x = width / 2.f;
    float y = height / 6.f;    // или чуть больше, если нужно ниже
    title.setPosition(x, y);


    // Подсказка
    hint.setFont(UsualFont);
    hint.setString("Use Arrow Keys or Mouse | Enter or Click");
    hint.setCharacterSize(20);
    hint.setFillColor(sf::Color(200, 200, 200));
    hint.setPosition(width / 2.f - 160, height - 50);

    // Кнопки меню
    std::vector<std::string> labels;
    if (SEscreen == 0) { labels = { "Start Game", "Instruction", "Exit" }; }
    if (SEscreen == 1) { labels = { "Restart", "Exit" }; }

    for (size_t i = 0; i < labels.size(); ++i) {
        sf::Text text;
        text.setFont(UsualFont);
        text.setString(labels[i]);
        text.setCharacterSize(40);

        // 1) вычисляем реальные границы текста
        sf::FloatRect tb = text.getLocalBounds();
        // 2) ставим origin в горизонтальный центр текста (и по верху, чтобы текст ровненько «сидел» на линии)
        text.setOrigin(tb.left + tb.width / 2.f, tb.top);
        // 3) позиционируем по центру окна по X, а по Y — от середины экрана с шагом i*70
        float x = width / 2.f;
        float y = height / 2.f + i * 70.f;
        text.setPosition(x, y);

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
    window.draw(bgSprite);

    window.draw(title);
    for (const auto& option : options)
        window.draw(option);
    window.draw(hint);
}

int MainMenu::getSelectedOption() const {
    return selectedIndex;
}