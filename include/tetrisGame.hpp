#pragma once

#include <gameMatrix.hpp>
#include <tetrominoFactory.hpp>
#include <bag.hpp>
#include <controller.hpp>

class TetrisGame {
private:
    GameMatrix gameMatrix;
    TetrominoFactory factory;
    Bag bag;
    Controller controller;
    int score;
    int frameCount;
    int level;
    int totalLinesCleared;

public:
    TetrisGame(int score, int frameCount, int level, int totalLinesCleared);

    void incrementLevel(int level=1);
    void incrementFrameCount(int frameCount=1);
    void calculateScore(int totalLinesCleared);

    int getScore() const noexcept;
    int getFrameCount() const noexcept;
    int getLevel() const;
    int getTotalLinesCleared() const;
};