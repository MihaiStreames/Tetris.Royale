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
    bool isColliding(const Tetromino& tetromino) const;
    bool trySpawnPiece(Tetromino& tetromino);
    bool tryPlacePiece(const Tetromino& tetromino);

    bool canMove(const Tetromino& tetromino, int dx, int dy) const;
    bool tryMoveCurrent(int dx, int dy) const;

    bool canRotate(const Tetromino& tetromino, bool clockwise) const;
    bool tryRotateCurrent(bool clockwise) const;

    bool tryMakeCurrentPieceFall() const { return tryMoveCurrent(0, 1); }

    int getRowstoObstacle(const Tetromino& tetromino) const;

    bool isLineFull(int line) const;
    void clearSingleLine(int line);
    int clearFullLines();

    const std::vector<std::vector<int>>& getBoard() const { return board; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};
