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

    [[nodiscard]] virtual bool isGameOver() const { return gameOver; }
    virtual void setGameOver(const bool flag) { gameOver = flag; }
    virtual void setEnergy(int setEnergy) { energy = setEnergy; }
    virtual void setBlockCommand(bool flag) { blockCommand = flag; }
    virtual void setReverseControlTimeCount(int nbr) { reverseControlTimeCount += nbr; }
    virtual void setActiveReverseControl(bool flag) { activeReverseControl = flag; }
    virtual void setDarkMode(bool flag) { isDarkMode = flag; }
    virtual void setDarkModeTimer(int time) { darkModeTimer = time; }

    virtual GameMatrix& getGameMatrix() { return gameMatrix; }
    virtual int getEnergy() { return energy; }
    virtual TetrisFactory& getFactory() { return factory; }
    virtual Bag& getBag()               { return bag; }
    virtual bool getReverseControls() { return reverseControls; }
    virtual bool getBlockCommand() { return blockCommand; }
    virtual int getReverseControlTimeCount() { return reverseControlTimeCount; }
    virtual bool getActiveReverseControl() { return activeReverseControl; }
    virtual bool getDarkMode() { return isDarkMode; }
    virtual int getDarkModeTimer() { return darkModeTimer; }

    [[nodiscard]] virtual int getFrameCount() const noexcept    { return frameCount; }
    virtual void setFrameCount(const int fc)                    { frameCount = fc; }
    virtual void incrementFrameCount(const int fc=1)            { frameCount += fc; }

    [[nodiscard]] virtual int getLevel() const noexcept { return level; }
    virtual void setLevel(const int lvl)                { level = lvl; }
    virtual void incrementLevel(const int lvl=1)        { level += lvl; }

    [[nodiscard]] virtual int getScore() const noexcept { return score; }

    virtual void calculateScore(const int linesCleared) {
        static const int tabScore[] = {0, 40, 100, 300, 1200};
        if (linesCleared >= 1 && linesCleared <= 4) score += tabScore[linesCleared];
    }

    virtual void calculateEnergy(const int linesCleared) {
        // casser une ligne donne 100 energies pour les tests (à changer à 10-15)
        static const int tabEnergy[] = {0, 100, 30, 40, 60, 100};
        if (linesCleared >= 1 && linesCleared <= 5)
            energy += tabEnergy[linesCleared];
    }

    [[nodiscard]] virtual int getLinesCleared() const               { return totalLinesCleared; }
    void setTotalLinesCleared(const int lines)              { totalLinesCleared = lines; }
    void incrementLinesCleared(const int q)                 { totalLinesCleared += q; }

    [[nodiscard]] virtual bool shouldApplyGravity() const {
        const int frame = getFrameCount();
        const int lvl = getLevel();
        int frame_quantum = 1;

        if (const auto it = SPEED_TABLE.find(lvl + speedFactor); it != SPEED_TABLE.end()) frame_quantum = it->second;

        return frame % frame_quantum == 0 && frame >= frame_quantum;
    }

    [[nodiscard]] virtual bool shouldLevelUp() const { return getLinesCleared() / LINES_TO_LEVELUP != getLevel(); }

    virtual void updateLevelAfterLineClear() {
        if (shouldLevelUp()) {
            const int newLineBasedLevel = getLinesCleared() / LINES_TO_LEVELUP;
            if (const int diff = newLineBasedLevel - getLevel(); diff > 0) incrementLevel(diff);
        }
    }

    // Malus Method
    virtual void inverted_command(bool flag) { std::cerr << "You tried to acces power-ups via TetrisGame"; }

    virtual void block_command() { std::cerr << "You tried to acces power-ups via TetrisGame"; }

    virtual void thunder_strike() { std::cerr << "You tried to acces power-ups via TetrisGame"; }
    virtual void fast_falling_pieces() { std::cerr << "You tried to acces power-ups via TetrisGame"; }
    virtual void darkMode() { std::cerr << "You tried to acces power-ups via TetrisGame"; }
    virtual void addPenaltyLines(int linesToAdd) { std::cerr << "You tried to acces power-ups via TetrisGame"; } 

    // Bonus Method
    virtual void blocs_1x1() { std::cerr << "You tried to acces power-ups via TetrisGame"; }
    virtual void slow_falling_pieces() { std::cerr << "You tried to acces power-ups via TetrisGame"; }
};
