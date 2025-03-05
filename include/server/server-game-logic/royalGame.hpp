#pragma once

#include "classicGame.hpp"

class RoyalGame final : public ClassicGame {

public:

    RoyalGame(const int gWidth, const int gHeight, const int gScore = 0, const int fc = 0, const int lvl = 0, const int totLinesCleared = 0): 
    ClassicGame(gWidth, gHeight, gScore, fc, lvl, totLinesCleared) {}
    ~RoyalGame() override {}


    // !! notation should be the SAME for all methods, I mean it also should be the same in the whole project
    // !! but at least in royalGame.hpp and royalGame.cpp
    
    void setInvertedFlag(bool flag) override;
    void thunderStrike() override;
    void blockControls() override;
    void fastPieces() override;
    void darkMode() override;
    void pushSingleBlock() override;
    void slowPieces() override;

};

