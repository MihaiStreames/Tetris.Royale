#pragma once

#include <map>
#include <iostream>
#include "gameMatrix.hpp"
#include "tetrisFactory.hpp"
#include "bag.hpp"

class TetrisGame {
protected:
    GameMatrix gameMatrix;
    TetrisFactory factory;
    Bag bag;
    int score;
    int reverseControlTimeCount = 0; // permet de compter cb de piece sont tombé
    int energy = 0;
    int frameCount;
    int level;
    int totalLinesCleared;
    int speedFactor = 0; // positif -> plus vite et negatif -> moins vite
    int darkModeTimer;


    bool gameOver = false;
    bool blockCommand = false; // voir s'il n y a pas une meilleur solution
    bool reverseControls = false;
    bool activeReverseControl = false; // -- 
    bool isDarkMode = false;


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
   virtual ~TetrisGame() {}

    [[nodiscard]] virtual bool isGameOver() const;
    virtual void setGameOver(const bool flag);
    virtual void setEnergy(int setEnergy);
    virtual void setBlockCommand(bool flag);
    virtual void setReverseControlTimeCount(int nbr);
    virtual void setActiveReverseControl(bool flag);
    virtual void setDarkMode(bool flag);
    virtual void setDarkModeTimer(int time);
    virtual GameMatrix& getGameMatrix();
    virtual int getEnergy();
    virtual TetrisFactory& getFactory();
    virtual Bag& getBag();
    virtual bool getReverseControls();
    virtual bool getBlockCommand();
    virtual int getReverseControlTimeCount();
    virtual bool getActiveReverseControl();
    virtual bool getDarkMode();
    virtual int getDarkModeTimer();

    [[nodiscard]] virtual int getFrameCount() const noexcept;
    virtual void setFrameCount(const int fc);
    virtual void incrementFrameCount(const int fc=1);

    [[nodiscard]] virtual int getLevel() const noexcept;
    virtual void setLevel(const int lvl);
    virtual void incrementLevel(const int lvl=1);

    [[nodiscard]] virtual int getScore() const noexcept;

    virtual void calculateScore(const int linesCleared);

    virtual void calculateEnergy(const int linesCleared);

    [[nodiscard]] virtual int getLinesCleared() const;
    void setTotalLinesCleared(const int lines);
    void incrementLinesCleared(const int q);

    [[nodiscard]] virtual bool shouldApplyGravity() const;

    [[nodiscard]] virtual bool shouldLevelUp() const;

    virtual void updateLevelAfterLineClear();

    // Malus Method
    virtual void inverted_command(bool flag);

    virtual void block_command();

    virtual void thunder_strike();
    virtual void fast_falling_pieces();
    virtual void darkMode();
    virtual void addPenaltyLines(int linesToAdd);

    // Bonus Method
    virtual void blocs_1x1();
    virtual void slow_falling_pieces();
};
