#pragma once

#include <cstdlib>
#include "tetrisGame.hpp"

class ClassicGame : public TetrisGame
{
    // J'ai retiré le gameOver puisqu'on y a acces depuis son parent
    std::vector<ClassicGame *> opponents;

public:
    ClassicGame(const int gWidth, const int gHeight, const int gScore = 0, const int fc = 0, const int lvl = 0, const int totLinesCleared = 0): 
    TetrisGame(gWidth, gHeight, gScore, fc, lvl, totLinesCleared) {}
    ~ClassicGame() {}

    virtual std::vector<ClassicGame*> getOpponents();

    virtual void addPenaltyLines(int linesToAdd);

    // permet de switch l'affichage entre les opposants
    void changePlayerView();
};