#pragma once

#include "classicGame.hpp"

class RoyalGame : public ClassicGame {

public:

    RoyalGame(const int gWidth, const int gHeight, const int gScore = 0, const int fc = 0, const int lvl = 0, const int totLinesCleared = 0): 
    ClassicGame(gWidth, gHeight, gScore, fc, lvl, totLinesCleared) {}
    ~RoyalGame() {}


    // !! notation should be the SAME for all methods, I mean it also should be the same in the whole project
    // !! but at least in royalGame.hpp and royalGame.cpp
    
    void inverted_command(bool flag) override;
    void thunder_strike() override;
    void block_command() override;
    void fast_falling_pieces() override;
    void darkMode() override;
    void blocs_1x1() override;
    void slow_falling_pieces() override;

};

