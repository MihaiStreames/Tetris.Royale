#pragma once

#include "gameEngine.hpp"
#include "classicGame.hpp"

class ClassicEngine : public GameEngine {
public:

    ClassicEngine(): GameEngine() {}

    // trouver un endroit ou faire cette appel
    void sendToEnemy(int linesCleared, TetrisGame& enemyGame) override;

};