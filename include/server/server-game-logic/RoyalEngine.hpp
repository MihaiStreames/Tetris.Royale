#pragma once

#include "GameEngine.hpp"
#include "RoyalGame.hpp"

class RoyalEngine final : public GameEngine
{
    const int INVERTED_CONTROLS_COOLDOWN = 3;
    const int POWER_UP_COST = 100;

  public:
    RoyalEngine() = default;
    ~RoyalEngine() override = default;

    [[nodiscard]] virtual bool handleAction(TetrisGame& game, Action action) override;
    [[nodiscard]] virtual bool viewPreviousOpponent(TetrisGame& game) override;
    [[nodiscard]] virtual bool viewNextOpponent(TetrisGame& game) override;

    [[nodiscard]] bool hasEnoughEnergy(RoyalGame& game);
    static void handleEnergy(TetrisGame& game, const int linesCleared);
    void handleGameLogic(TetrisGame& game) override;
    void handlingRoutine(TetrisGame& game, Action action) override;
    void handleGameFlags(RoyalGame& game);
    void handleBonus(TetrisGame& game) override;
    void handleMalus(TetrisGame& game) override;
    
};
