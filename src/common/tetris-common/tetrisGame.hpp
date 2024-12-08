#pragma once

#include "gameMatrix.hpp"
#include "tetrisFactory.hpp"
#include "bag.hpp"

class TetrisGame {
    GameMatrix gameMatrix;
    TetrisFactory factory;
    Bag bag;
    int score;
    int frameCount;
    int level;
    int totalLinesCleared;

public:
    TetrisGame(const int gWidth, const int gHeight, const int gScore, const int fc, const int lvl, const int totLinesCleared)
    : gameMatrix(gWidth, gHeight), score(gScore), frameCount(fc), level(lvl), totalLinesCleared(totLinesCleared) { }

    void incrementLevel(const int lvl=1) { level += lvl; }
    void incrementFrameCount(const int fc=1) { frameCount += fc; }

    void calculateScore(const int linesCleared) {
        static const int tabScore[] = {0, 40, 100, 300, 1200};
        if (linesCleared >= 1 && linesCleared <= 4) {
            score += tabScore[linesCleared];
        }
    }

    [[nodiscard]] int getScore() const noexcept { return score; }
    [[nodiscard]] int getFrameCount() const noexcept { return frameCount; }
    [[nodiscard]] int getLevel() const noexcept { return level; }
    [[nodiscard]] int getTotalLinesCleared() const noexcept { return totalLinesCleared; }
};