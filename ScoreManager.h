#pragma once
#include <string>

class ScoreManager {
public:
    static void saveScore(int score);
    static int loadHighScore();

private:
    static const std::string filename;
};

