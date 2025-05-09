#pragma once

#include "ClassicGame.hpp"
#include "GameEngine.hpp"

class ClassicEngine final : public GameEngine
{
    const int MAX_COMBO = 4;

  public:
    // no need to redefine the constructor
    ClassicEngine() = default;

    void handleBasicPenalty(ClassicGame& game, int linesCleared);
    void handleGameLogic(TetrisGame& game) override;
    
    [[nodiscard]] virtual bool handleAction(TetrisGame& game, Action action) override;
    [[nodiscard]] virtual bool viewPreviousOpponent(TetrisGame& game) override;
    [[nodiscard]] virtual bool viewNextOpponent(TetrisGame& game) override;

};
