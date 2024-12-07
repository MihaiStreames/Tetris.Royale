#pragma once

#include "gameMatrix.hpp"
#include "tetrisFactory.hpp"
#include "bag.hpp"
#include "controller.hpp"

class TetrisGame {
    GameMatrix gameMatrix;
    TetrisFactory factory;
    Bag bag;
    Controller controller;
    int score;
    int frameCount;
    int level;
    int totalLinesCleared;

public:
    TetrisGame(int gWidth, int gHeight, int gScore, int fc, int lvl, int totLinesCleared);

    void incrementLevel(const int lvl=1) { level += lvl; }
    void incrementFrameCount(const int fc=1) { frameCount += fc; }
    void calculateScore(int linesCleared);

    int getScore() const noexcept { return score; }
    int getFrameCount() const noexcept { return frameCount; }
    int getLevel() const noexcept { return level; }
    int getTotalLinesCleared() const noexcept { return totalLinesCleared; }
};