#include "tetrisGame.hpp"

using namespace std;

// Getters
int TetrisGame::getScore() const noexcept {
    return score;
}

int TetrisGame::getLevel() const {
    return level;
}

int TetrisGame::getFrameCount() const noexcept {
    return frameCount;
}

int TetrisGame::getTotalLinesCleared() const {
    return totalLinesCleared;
}

//Méthodes d'incrémentation
void TetrisGame::incrementLevel(int level) {
    this->level += level;
}

void TetrisGame::incrementFrameCount(int frameCount) {
    this->frameCount += frameCount;
}

// verification et calcul du score selon le nb de lignes compléter
void TetrisGame::calculateScore(int totalLinesCleared) {
    static const int tabScore[] = {0, 40, 100, 300, 1200};
    if (totalLinesCleared >= 1 && totalLinesCleared <= 4) {
        this->score += tabScore[totalLinesCleared];
    }
}