#pragma once

#include <map>
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
    int slowDownFactor = 0; // trouver une meilleur soution

    bool gameOver = false;
    bool blockCommand = false; // voir s'il n y a pas une meilleur solution
    bool reverseControls = false;
    bool activeReverseControl = false; // -- 

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

    [[nodiscard]] virtual bool isGameOver() const { return gameOver; }
    virtual void setGameOver(const bool flag) { gameOver = flag; }
    virtual void setEnergy(int setEnergy) { energy = setEnergy; }
    virtual void setBlockCommand(bool flag) { blockCommand = flag; }
    virtual void setReverseControlTimeCount(int nbr) { reverseControlTimeCount += nbr; }
    virtual void setActiveReverseControl(bool flag) { activeReverseControl = flag; }

    virtual GameMatrix& getGameMatrix() { return gameMatrix; }
    virtual int getEnergy() { return energy; }
    virtual TetrisFactory& getFactory() { return factory; }
    virtual Bag& getBag()               { return bag; }
    virtual bool getReverseControls() { return reverseControls; }
    virtual bool getBlockCommand() { return blockCommand; }
    virtual int getReverseControlTimeCount() { return reverseControlTimeCount; }
    virtual bool getActiveReverseControl() { return activeReverseControl; }

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

        if (const auto it = SPEED_TABLE.find(lvl - slowDownFactor); it != SPEED_TABLE.end()) frame_quantum = it->second;

        return frame % frame_quantum == 0 && frame >= frame_quantum;
    }

    [[nodiscard]] virtual bool shouldLevelUp() const { return getLinesCleared() / LINES_TO_LEVELUP != getLevel(); }

    virtual void updateLevelAfterLineClear() {
        if (shouldLevelUp()) {
            const int newLineBasedLevel = getLinesCleared() / LINES_TO_LEVELUP;
            if (const int diff = newLineBasedLevel - getLevel(); diff > 0) incrementLevel(diff);
        }
    }

    // Pour qu'on puisse appeler les methodes sans forcement savoir de quel mode de jeu ils viennent
    // A modifier, ce n'est pas une bonne pratique !!!
    virtual void inverted_command(bool flag) {
        reverseControls = flag;
    }

    virtual void thunder_strike()
    {
        // Choisir une colonne qui sera frappée par la foudre
        int col = rand() % gameMatrix.getWidth();

        // -1 = colonne ou se trouve aucune brique
        int impactRow = -1;

        tetroMat& board = gameMatrix.getBoard();

        for (int y = 0; y < gameMatrix.getHeight(); ++y)
        {
            if (board[y][col] == 1)
            { // Trouve la première brique
                impactRow = y;
                break;
            }
        }

        // Si aucun bloc touché, on ne fait rien
        if (impactRow == -1)
            return;

        // Détruire la zone 2x2 autour de l'impact
        for (int dy = 0; dy < 2; ++dy)
        {
            for (int dx = 0; dx < 2; ++dx)
            {
                int x = col + dx;
                int y = impactRow + dy;
                if (x < gameMatrix.getWidth() && y < gameMatrix.getHeight())
                {                    
                    board[y][x] = 0;
                }
            }
        }
    }

    virtual void block_command() {
        blockCommand = !blockCommand;
    }

    virtual void fast_falling_pieces() {}
    virtual void light_off() {}

    virtual void blocs_1x1() {}
    virtual void slow_falling_pieces() {
        if (level > 0) {
            slowDownFactor++;
            if (level - slowDownFactor < 0) { // verifier si c est correct
                slowDownFactor = 0;
            }
        }
    }
};
