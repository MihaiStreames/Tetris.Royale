#pragma once

#include <cstdlib>
#include "tetrisGame.hpp"

class ClassicGame : public TetrisGame {

private:

    int getOpponentVectorSize();

    std::vector<ClassicGame*> opponents;
    int targetIndex;

public:

    ClassicGame(const int gWidth, const int gHeight, const int gScore = 0, const int fc = 0, const int lvl = 0, const int totLinesCleared = 0);
    ~ClassicGame() override = default;

    [[nodiscard]] virtual std::vector<ClassicGame*> getOpponents();
    void setOpponents(std::vector<ClassicGame*> opponents);
    [[nodiscard]] virtual ClassicGame* getTarget();

    void addPenaltyLines(int linesToAdd) override;
    void changePlayerView(int idx);

};

