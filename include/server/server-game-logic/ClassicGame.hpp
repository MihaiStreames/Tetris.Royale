#pragma once

#include "TetrisGame.hpp"

#include <cstdlib>

class ClassicGame : public TetrisGame
{
  private:
    int getOpponentVectorSize();

    std::vector<TetrisGame*> opponents;
    int targetIndex;

  public:
    ClassicGame(const int gWidth, const int gHeight, const int gScore = 0,
                const int fc = 0, const int lvl = 0,
                const int totLinesCleared = 0);
    ~ClassicGame() override = default;

    [[nodiscard]] virtual std::vector<TetrisGame*> getOpponents() override;
    [[nodiscard]] virtual TetrisGame* getTarget() override;
    [[nodiscard]] virtual int getTargetIndex() override;
    void addOpponent(TetrisGame* opponent) override;
    void removeOpponent(TetrisGame* opponent) override;

    void addPenaltyLines(int linesToAdd) override;
    void changePlayerView(int idx) override;
};
