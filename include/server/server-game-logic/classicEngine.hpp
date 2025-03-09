#pragma once

#include "gameEngine.hpp"
#include "classicGame.hpp"


class ClassicEngine final : public GameEngine {

    const int MAX_COMBO = 4;

public:

    // no need to redefine the constructor
    ClassicEngine() = default;


    void handleBasicPenalty(ClassicGame& game, int linesCleared);
    void handleGameLogic(TetrisGame& game) override;

    [[nodiscard]] virtual bool viewPreviousOpponent(TetrisGame& game) override;
    [[nodiscard]] virtual bool viewNextOpponent(TetrisGame& game) override;

};

