#pragma once

#include "ClassicGame.hpp"

class RoyalGame final : public ClassicGame
{
    const int THNUDERSTRIKE_BLAST_RADIUS = 2;
    const int SINGLE_BLOCKS_TO_PUSH = 2;
    const int DARK_MODE_TIMER = 150;

  public:
    RoyalGame(const int gWidth, const int gHeight, const int gScore = 0,
              const int fc = 0, const int lvl = 0,
              const int totLinesCleared = 0, const std::string &name = DEFAULT_NAME);
    ~RoyalGame() override = default;

    void spawnThunderStrike() override;
    void increaseFallingSpeed() override;
    void decreaseFallingSpeed() override;
    void pushSingleBlock() override;
    void startDarkMode() override;
    void startBlockControls() override;
    void startInvertedControls() override;
};
