#pragma once

#include <vector>
#include "tetromino.hpp"

class GameMatrix {
    Tetromino* currentTetromino;
    int width;
    int height;
    std::vector<std::vector<int>> board;

public:
    GameMatrix(int wMatrix, int hMatrix);

    void generateBoardByDimension() { board = std::vector(height, std::vector(width, 0)); }
    [[nodiscard]] bool isColliding(const Tetromino& tetromino) const;
    bool trySpawnPiece(Tetromino& tetromino);
    bool tryPlacePiece(const Tetromino& tetromino);

    [[nodiscard]] bool canMove(const Tetromino& tetromino, int dx, int dy) const;
    [[nodiscard]] bool tryMoveCurrent(int dx, int dy) const;

    [[nodiscard]] bool canRotate(const Tetromino& tetromino, bool clockwise) const;
    [[nodiscard]] bool tryRotateCurrent(bool clockwise) const;

    [[nodiscard]] bool tryMakeCurrentPieceFall() const { return tryMoveCurrent(0, 1); }

    [[nodiscard]] int getRowstoObstacle(const Tetromino& tetromino) const;

    [[nodiscard]] bool isLineFull(int line) const;
    void clearSingleLine(int line);
    int clearFullLines();

    [[nodiscard]] const std::vector<std::vector<int>>& getBoard() const { return board; }
    [[nodiscard]] int getWidth() const  { return width; }
    [[nodiscard]] int getHeight() const { return height; }
};
