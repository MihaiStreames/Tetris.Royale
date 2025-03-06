#pragma once

#include "gameEngine.hpp"
#include "royalGame.hpp"

class RoyalEngine final : public GameEngine {

    const int INVERTED_CONTROLS_COOLDOWN = 3;
    const int POWER_UP_COST = 100;

public:

    RoyalEngine() = default;
    ~RoyalEngine() override = default;

    bool handleAction(TetrisGame &game, Action action) override;
    void handleGameLogic(TetrisGame &game) override;

    static void handleEnergy(TetrisGame &game, const int linesCleared);
    [[nodiscard]] bool hasEnoughEnergy(RoyalGame &game);
    void handlingRoutine(TetrisGame &game, Action action) override;
    void handleGameFlags(RoyalGame &game);
    
    void handleBonus(TetrisGame &game) override;
    void handleMalus(TetrisGame &game) override;
    

};

