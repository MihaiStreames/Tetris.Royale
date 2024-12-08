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
    TetrisGame(int gWidth, int gHeight, int gScore, int fc, int lvl, int totLinesCleared)
    : gameMatrix(gWidth, gHeight), score(gScore), frameCount(fc), level(lvl), totalLinesCleared(totLinesCleared) { }

    void incrementLevel(const int lvl=1) { level += lvl; }
    void incrementFrameCount(const int fc=1) { frameCount += fc; }

    void calculateScore(int linesCleared) {
        static const int tabScore[] = {0, 40, 100, 300, 1200};
        if (linesCleared >= 1 && linesCleared <= 4) {
            score += tabScore[linesCleared];
        }
    }

    int getScore() const noexcept { return score; }
    int getFrameCount() const noexcept { return frameCount; }
    int getLevel() const noexcept { return level; }
    int getTotalLinesCleared() const noexcept { return totalLinesCleared; }
};