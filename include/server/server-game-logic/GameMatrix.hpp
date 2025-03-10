#pragma once

#include "Tetromino.hpp"

#include <algorithm>
#include <iostream>
#include <optional>
#include <random>
#include <vector>

class GameMatrix
{
  private:
    std::optional<Tetromino> currentTetromino;
    int width;
    int height;
    tetroMat board;

  public:
    GameMatrix(const int wMatrix, const int hMatrix);
    ~GameMatrix() = default;

    // getters
    [[nodiscard]] const Tetromino* getCurrent() const;
    [[nodiscard]] int getWidth() const;
    [[nodiscard]] int getHeight() const;
    [[nodiscard]] tetroMat& getBoard();
    [[nodiscard]] tetroMat getBoardWithCurrentPiece() const;

    // setters if needed
    void setCurrent(const Tetromino& tetromino);

    // move related methods
    [[nodiscard]] bool canMove(const Tetromino& tetromino, int dx, int dy);
    [[nodiscard]] bool tryMoveCurrent(int dx, int dy);
    [[nodiscard]] bool tryMoveLeft();
    [[nodiscard]] bool tryMoveRight();
    [[nodiscard]] bool tryMoveDown();
    [[nodiscard]] bool tryInstantFall();

    // rotation related methods
    [[nodiscard]] bool canRotate(const Tetromino& tetromino, bool clockwise);
    [[nodiscard]] bool tryRotateCurrent(bool clockwise);
    [[nodiscard]] bool tryRotateLeft();
    [[nodiscard]] bool tryRotateRight();

    // current piece logic related methods
    [[nodiscard]] bool tryMakeCurrentPieceFall();
    [[nodiscard]] bool tryPlacePiece(const Tetromino& tetromino);
    [[nodiscard]] bool tryPlaceCurrentPiece();
    [[nodiscard]] bool trySpawnPiece(Tetromino piece);
    void deleteCurrent();

    // board check related methods
    [[nodiscard]] bool isTileEmpty(int x, int y);
    [[nodiscard]] bool isColliding(const Tetromino& tetromino);
    [[nodiscard]] bool isLineFull(int line);
    [[nodiscard]] bool isLineEmpty(int line);
    [[nodiscard]] bool areLinesEmpty(int start, int end);
    [[nodiscard]] int getRowsToObstacle(const Tetromino& tetromino);
    [[nodiscard]] int findHighestBlockInColumn(int col);

    // board manipulation related methods
    void clearSingleLine(int line);
    [[nodiscard]] int clearFullLines();
    void pushNewLinesAtBottom(std::vector<std::vector<int>> newLines);
    void pushPenaltyLinesAtBottom(int linesToAdd);
    void destroyAreaAroundBlock(const Position2D pos, const int blastRadius);

    // util
    static tetroMat generateBoardByDimension(int width, int height);
};
