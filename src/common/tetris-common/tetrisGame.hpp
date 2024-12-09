#pragma once

#include <map>
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

    bool gameOver = false;

    static constexpr int LINES_TO_LEVELUP = 10;

    inline static const std::map<int,int> SPEED_TABLE = {
        {0,48},{1,43},{2,38},{3,33},{4,28},{5,23},{6,18},{7,13},{8,8},{9,6},
        {10,5},{11,5},{12,5},
        {13,4},{14,4},{15,4},
        {16,3},{17,3},{18,3},
        {19,2},{20,2},{21,2},{22,2},{23,2},{24,2},{25,2},{26,2},{27,2},{28,2},
        {29,1}
    };

public:
    TetrisGame(const int gWidth, const int gHeight, const int gScore = 0, const int fc = 0, const int lvl = 0, const int totLinesCleared = 0)
   : gameMatrix(gWidth, gHeight), score(gScore), frameCount(fc), level(lvl), totalLinesCleared(totLinesCleared) {}

    [[nodiscard]] bool isGameOver() const { return gameOver; }
    void setGameOver(const bool flag) { gameOver = flag; }

    GameMatrix& getGameMatrix() { return gameMatrix; }
    TetrisFactory& getFactory() { return factory; }
    Bag& getBag()               { return bag; }

    [[nodiscard]] int getFrameCount() const noexcept    { return frameCount; }
    void setFrameCount(const int fc)                    { frameCount = fc; }
    void incrementFrameCount(const int fc=1)            { frameCount += fc; }

    [[nodiscard]] int getLevel() const noexcept { return level; }
    void setLevel(const int lvl)                { level = lvl; }
    void incrementLevel(const int lvl=1)        { level += lvl; }

    [[nodiscard]] int getScore() const noexcept { return score; }

    void calculateScore(const int linesCleared) {
        static const int tabScore[] = {0, 40, 100, 300, 1200};
        if (linesCleared >= 1 && linesCleared <= 4) score += tabScore[linesCleared];
    }

    [[nodiscard]] int getLinesCleared() const               { return totalLinesCleared; }
    void setTotalLinesCleared(const int lines)              { totalLinesCleared = lines; }
    void incrementLinesCleared(const int q)                 { totalLinesCleared += q; }

    [[nodiscard]] bool shouldApplyGravity() const {
        const int frame = getFrameCount();
        const int lvl = getLevel();
        int frame_quantum = 1;

        if (const auto it = SPEED_TABLE.find(lvl); it != SPEED_TABLE.end()) frame_quantum = it->second;

        return frame % frame_quantum == 0 && frame >= frame_quantum;
    }

    [[nodiscard]] bool shouldLevelUp() const { return getLinesCleared() / LINES_TO_LEVELUP != getLevel(); }

    void updateLevelAfterLineClear() {
        if (shouldLevelUp()) {
            const int newLineBasedLevel = getLinesCleared() / LINES_TO_LEVELUP;
            if (const int diff = newLineBasedLevel - getLevel(); diff > 0) incrementLevel(diff);
        }
    }
};