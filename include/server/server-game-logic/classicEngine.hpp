#pragma once

#include "gameEngine.hpp"
#include "classicGame.hpp"


class ClassicEngine final : public GameEngine {

public:

    ClassicEngine() = default;
    void handleBasicPenalty(ClassicGame& game, int linesCleared);
    void handleGameLogic(RoyalGame &game) override;

};

