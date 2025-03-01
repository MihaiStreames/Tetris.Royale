#pragma once

#include "gameEngine.hpp"
#include "royalGame.hpp"

class RoyalEngine : public GameEngine {
public:
    RoyalEngine(): GameEngine() {}

    bool handleAction(TetrisGame& game, const Action action) override;

    bool handlePlacingPiece(TetrisGame& game) override;

    void handleGameLogic(TetrisGame& game) override;

   void handleEnergy(TetrisGame &game, const int linesCleared) override;

    void handlingRoutine(TetrisGame& game, const Action action) override;

    bool handleBonus(TetrisGame &game) override;

    bool handleMalus(TetrisGame &game) override;
};