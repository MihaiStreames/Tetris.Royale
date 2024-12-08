#include "tetrisGame.hpp"

TetrisGame::TetrisGame(const int gWidth, const int gHeight, const int gScore, const int fc, const int lvl, const int totLinesCleared)
    : gameMatrix(gWidth, gHeight), score(gScore), frameCount(fc), level(lvl), totalLinesCleared(totLinesCleared) {}

void TetrisGame::calculateScore(const int linesCleared) {
    static const int tabScore[] = {0, 40, 100, 300, 1200};
    if (linesCleared >= 1 && linesCleared <= 4) {
        score += tabScore[linesCleared];
    }
}