#pragma once

#include "types.hpp"
#include "tetrisGame.hpp"


class PowerUp {
protected:
    // std::vector<TypePowerUps> activePowers; -> dans la gameEngine
    int duration;
    int timeLeft;

public:
    PowerUp(int duration): duration{duration}, timeLeft{duration} {}
    virtual ~PowerUp() {}

    virtual void applyEffect(TetrisGame &tg)=0;
    virtual void removeEffect(TetrisGame &tg)=0;

    void updateTimer(int deltaTime) {
        timeLeft -= deltaTime;
    }

    bool isExpired() { return timeLeft <= 0;}
};

class InvertedCommand : public PowerUp {
public:
    InvertedCommand(): PowerUp(3000) {} // 3000 -> 3 sec

    void applyEffect(TetrisGame &tg) override {
        tg.invertCmd(true);
    }

    void removeEffect(TetrisGame &tg) override {
        tg.invertCmd(false);
    }
};