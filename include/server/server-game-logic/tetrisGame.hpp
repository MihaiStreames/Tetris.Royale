#pragma once


#include <map>
#include <iostream>


#include "gameMatrix.hpp"
#include "tetrisFactory.hpp"
#include "bag.hpp"



class TetrisGame {

    const int LINES_TO_LEVELUP = 10;
    const std::map<int, int> SPEED_TABLE = {
        {0,48}, {1,43}, {2,38}, {3,33}, {4,28}, {5,23}, {6,18}, {7,13}, {8,8}, {9,6},
        {10,5}, {11,5}, {12,5},
        {13,4}, {14,4}, {15,4},
        {16,3}, {17,3}, {18,3},
        {19,2}, {20,2}, {21,2}, {22,2}, {23,2}, {24,2}, {25,2}, {26,2}, {27,2}, {28,2},
        {29,1}
    };

    const int MAX_LINES_CLEARABLE_AT_ONCE = 4;

    const int ScoreRewardTab[5] = {0, 40, 100, 300, 1200};
    const int EnergyRewardTab[5] = {0, 30, 40, 60, 100};
    


protected:

    // Tetris Game Components
    GameMatrix gameMatrix;
    TetrisFactory factory;
    Bag bag;
    int score;
    
    // some game variables
    int frameCount;
    int level;
    int totalLinesCleared;
    bool gameOver = false;

    // some logic variables for powers ups
    int energy = 0;
    int darkModeTimer = -1;  // default
    int speedFactor = 0;
    int malusCooldown = 0;

    // flags for powers ups
    bool blockControlsFlag = false;
    bool reverseControlsFlag = false;
    bool darkModeFlag = false;



public:

    TetrisGame(const int gWidth, const int gHeight, const int gScore = 0, const int fc = 0, const int lvl = 0, const int totLinesCleared = 0);
    virtual ~TetrisGame() = default;

    // getters
    [[nodiscard]] virtual GameMatrix& getGameMatrix();
    [[nodiscard]] virtual TetrisFactory& getFactory();
    [[nodiscard]] virtual Bag& getBag();
    [[nodiscard]] virtual int getScore() const noexcept;

    [[nodiscard]] virtual int getFrameCount() const noexcept;
    [[nodiscard]] virtual int getLevel() const noexcept;
    [[nodiscard]] virtual int getLinesCleared() const noexcept;
    [[nodiscard]] virtual bool isGameOver() const noexcept;

    [[nodiscard]] virtual int getEnergy() const noexcept;
    [[nodiscard]] virtual int getDarkModeTimer() const noexcept;
    [[nodiscard]] virtual int getSpeedFactor() const noexcept;
    [[nodiscard]] virtual int getMalusCooldown() const noexcept;

    [[nodiscard]] virtual bool getBlockControlsFlag() const noexcept;
    [[nodiscard]] virtual bool getReverseControlsFlag() const noexcept;
    [[nodiscard]] virtual bool getDarkModeFlag() const noexcept;


    // special getters
    [[nodiscard]] Tetromino& getNextPiece();
    [[nodiscard]] const Tetromino* getHoldPiece() const;
    
    
    // setters
    virtual void setScore(const int s);

    virtual void setFrameCount(const int fc);
    virtual void setLevel(const int lvl);
    virtual void setTotalLinesCleared(const int lines);
    virtual void setGameOver(const bool flag);

    virtual void setEnergy(int setEnergy);
    virtual void setDarkModeTimer(int time);
    virtual void setSpeedFactor(int speed);
    virtual void setMalusCooldown(int nbr);

    virtual void setBlockControlsFlag(bool flag);
    virtual void setReverseControlsFlag(bool flag);
    virtual void setDarkModeFlag(bool flag);


    // !! swap following methods for consistency sake
    // update stuff (mostly incrementation)
    virtual void updateScore(const int linesCleared);
    virtual void updateEnergy(const int linesCleared);

    virtual void incrementLinesCleared(const int q);
    virtual void incrementScore(const int sc);
    virtual void incrementFrameCount(const int fc);
    virtual void incrementLevel(const int lvl);
    virtual void incrementEnergy(int incr);
    virtual void incrementMalusCooldown(int nbr);
    // !! end of swap

    // logic stuff
    [[nodiscard]] virtual bool shouldApplyGravity() const;
    [[nodiscard]] virtual bool shouldLevelUp() const;
    virtual void updateLevelAfterLineClear();
    
    
    // powers up thingy
    virtual void addPenaltyLines(int linesToAdd);
    virtual void spawnThunderStrike();
    virtual void increaseFallingSpeed();
    virtual void decreaseFallingSpeed();
    virtual void startDarkMode();
    virtual void pushSingleBlock();
    
};

