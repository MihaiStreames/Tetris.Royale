#pragma once

#include "Bag.hpp"
#include "GameMatrix.hpp"
#include "TetrisFactory.hpp"
#include "Common.hpp"

#include <iostream>
#include <map>

class TetrisGame
{
    const int LINES_TO_LEVELUP = 10;
    const std::map<int, int> SPEED_TABLE = {
        {0, 48}, {1, 43}, {2, 38}, {3, 33}, {4, 28}, {5, 23}, {6, 18}, {7, 13},
        {8, 8},  {9, 6},  {10, 5}, {11, 5}, {12, 5}, {13, 4}, {14, 4}, {15, 4},
        {16, 3}, {17, 3}, {18, 3}, {19, 2}, {20, 2}, {21, 2}, {22, 2}, {23, 2},
        {24, 2}, {25, 2}, {26, 2}, {27, 2}, {28, 2}, {29, 1},
    };

    const int MAX_LINES_CLEARABLE_AT_ONCE = 4;
    const int MAX_ENERGY = 500;

    const int ScoreRewardTab[5] = {0, 40, 100, 300, 1200};
    const int EnergyRewardTab[5] = {0, 30, 40, 60, 100};

  protected:
    // Tetris Game Components
    GameMatrix gameMatrix;
    TetrisFactory factory;
    Bag bag;
    int score;

    // some game variables
    int frameCount;
    int level;
    int totalLinesCleared;
    bool gameOver = false;

    // some logic variables for powers ups
    int energy = 0;
                             // bonus/malus (set to 0 in prod)
    int darkModeTimer = -1;  // default
    int speedFactor = 0;
    int malusCooldown = 0;

    // name of the player
    std::string playerName;

    // flags for powers ups
    bool blockControlsFlag = false;
    bool reverseControlsFlag = false;
    bool darkModeFlag = false;
    GameMode gameMode;

  public:
    TetrisGame(const int gWidth, const int gHeight, const int gScore = 0,
               const int fc = 0, const int lvl = 0,
               const int totLinesCleared = 0, const std::string& name = DEFAULT_NAME);
    virtual ~TetrisGame() = default;

    // getters
    [[nodiscard]] virtual GameMatrix& getGameMatrix();
    [[nodiscard]] virtual TetrisFactory& getFactory();
    [[nodiscard]] virtual Bag& getBag();
    [[nodiscard]] virtual int getScore() const noexcept;
    [[nodiscard]] virtual std::string getPlayerName() const noexcept;
    [[nodiscard]] virtual GameMode getGameMode() noexcept;

    [[nodiscard]] virtual int getFrameCount() const noexcept;
    [[nodiscard]] virtual int getLevel() const noexcept;
    [[nodiscard]] virtual int getLinesCleared() const noexcept;
    [[nodiscard]] virtual bool isGameOver() const noexcept;

    [[nodiscard]] virtual int getEnergy() const noexcept;
    [[nodiscard]] virtual int getDarkModeTimer() const noexcept;
    [[nodiscard]] virtual int getSpeedFactor() const noexcept;
    [[nodiscard]] virtual int getMalusCooldown() const noexcept;

    [[nodiscard]] virtual bool getBlockControlsFlag() const noexcept;
    [[nodiscard]] virtual bool getReverseControlsFlag() const noexcept;
    [[nodiscard]] virtual bool getDarkModeFlag() const noexcept;

    // special getters
    [[nodiscard]] Tetromino& getNextPiece();
    [[nodiscard]] const Tetromino* getHoldPiece() const;

    // opponents stuff
    [[nodiscard]] virtual std::vector<TetrisGame*> getOpponents();
    [[nodiscard]] virtual TetrisGame* getTarget();
    [[nodiscard]] virtual int getTargetIndex();
    void virtual addOpponent(TetrisGame* opponent);
    void virtual removeOpponent(TetrisGame* opponent);
    void virtual changePlayerView(int idx);

    // setters
    virtual void setScore(const int s);

    virtual void setFrameCount(const int fc);
    virtual void setLevel(const int lvl);
    virtual void setTotalLinesCleared(const int lines);
    virtual void setGameOver(const bool flag);
    virtual void setPlayerName(const std::string& name);
    virtual void setGameMode(const GameMode mode);

    virtual void setEnergy(int setEnergy);
    virtual void setDarkModeTimer(int time);
    virtual void setSpeedFactor(int speed);
    virtual void setMalusCooldown(int nbr);

    virtual void setBlockControlsFlag(bool flag);
    virtual void setReverseControlsFlag(bool flag);
    virtual void setDarkModeFlag(bool flag);

    // increment stuff
    virtual void incrementScore(const int sc);
    virtual void incrementFrameCount(const int fc);
    virtual void incrementLevel(const int lvl);
    virtual void incrementLinesCleared(const int q);
    virtual void incrementEnergy(int incr);
    virtual void incrementMalusCooldown(int nbr);

    // some calculation methods (increment... kinda)
    virtual void updateScore(const int linesCleared);
    virtual void updateEnergy(const int linesCleared);

    // logic stuff
    [[nodiscard]] virtual bool shouldApplyGravity() const;
    [[nodiscard]] virtual bool shouldLevelUp() const;
    virtual void updateLevelAfterLineClear();

    // powers up thingy
    virtual void addPenaltyLines(int linesToAdd);
    virtual void spawnThunderStrike();
    virtual void increaseFallingSpeed();
    virtual void decreaseFallingSpeed();
    virtual void startDarkMode();
    virtual void startBlockControls();
    virtual void startInvertedControls();
    virtual void pushSingleBlock();
};
