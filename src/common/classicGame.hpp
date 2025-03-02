#pragma once

#include <cstdlib>
#include "tetrisGame.hpp"

class ClassicGame : public TetrisGame
{
    // J'ai retiré le gameOver puisqu'on y a acces depuis son parent
    std::vector<ClassicGame *> opponents;
    ClassicGame* target;
    int targetIndex;

public:
    ClassicGame(const int gWidth, const int gHeight, const int gScore = 0, const int fc = 0, const int lvl = 0, const int totLinesCleared = 0): 
    TetrisGame(gWidth, gHeight, gScore, fc, lvl, totLinesCleared) {
        // faire une requete qui demande au seveur la liste des joueurs
        // target = opponents[0];
        // targetIndex = 0;
    }

    ~ClassicGame() {}

    virtual std::vector<ClassicGame*> getOpponents();

    virtual ClassicGame* getTarget();

    virtual void addPenaltyLines(int linesToAdd);

    // permet de switch l'affichage entre les opposants
    void changePlayerView();
};