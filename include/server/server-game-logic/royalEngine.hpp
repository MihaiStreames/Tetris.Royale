#pragma once

#include "gameEngine.hpp"
#include "royalGame.hpp"

class RoyalEngine final : public GameEngine {

public:

    RoyalEngine() {}

    bool handleAction(RoyalGame &game, Action action) override;
    bool handlePlacingPiece(RoyalGame &game) override;

    void handleGameLogic(RoyalGame &game) override;

    static void handleEnergy(RoyalGame &game, const int linesCleared) { game.calculateEnergy(linesCleared); }
    void handlingRoutine(RoyalGame &game, Action action) override;

    bool handleBonus(RoyalGame &game) override;
    bool handleMalus(RoyalGame &game) override;

};

