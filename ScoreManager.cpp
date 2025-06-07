#include "ScoreManager.h"
#include <fstream>
#include <algorithm>
#include <iostream>

const std::string ScoreManager::filename = "highscore.txt";

void ScoreManager::saveScore(int score) {
    std::ifstream in(filename);
    if (!in) return; // если нет файла — ничего не делаем

    int currentHigh;
    in >> currentHigh;
    in.close();

    if (score > currentHigh) {
        std::ofstream out(filename);
        if (out)
            out << score;
        int highScore = loadHighScore();
        if (score > highScore) {
            std::ofstream file(filename);
            if (file.is_open()) {
                file << score;
            }
        }
    }
}

    int ScoreManager::loadHighScore() {
        std::ifstream file(filename);
        int score = 0;
        if (file.is_open()) {
            file >> score;
        }
        return score;
    }
