#pragma once

#include "gameEngine.hpp"
#include "classicGame.hpp"

class ClassicEngine : public GameEngine {
public:

    ClassicEngine(): GameEngine() {}

    void handleBasicPenalty(ClassicGame& game, const int linesCleared);

    void handleGameLogic(TetrisGame& game) override;
};