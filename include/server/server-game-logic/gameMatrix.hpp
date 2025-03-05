#pragma once

#include <vector>
#include <optional>
#include "tetromino.hpp"

class GameMatrix {
    std::optional<Tetromino> currentTetromino;
    int width;
    int height;
    tetroMat board;

public:
    GameMatrix(const int wMatrix, const int hMatrix)
        : currentTetromino(std::nullopt), width(wMatrix), height(hMatrix), board(hMatrix, std::vector(wMatrix, 0)) {
    }

    void generateBoardByDimension() { board = std::vector(height, std::vector(width, 0)); }

    [[nodiscard]] bool isColliding(const Tetromino &tetromino) const;

    bool trySpawnPiece(Tetromino piece);

    bool tryPlacePiece(const Tetromino &tetromino);

    bool tryPlaceCurrentPiece();

    [[nodiscard]] bool canMove(const Tetromino &tetromino, int dx, int dy) const;

    [[nodiscard]] bool tryMoveCurrent(int dx, int dy);

    [[nodiscard]] bool canRotate(const Tetromino &tetromino, bool clockwise) const;

    [[nodiscard]] bool tryRotateCurrent(bool clockwise);

    [[nodiscard]] bool tryMakeCurrentPieceFall() { return tryMoveCurrent(0, 1); }

    [[nodiscard]] int getRowsToObstacle(const Tetromino &tetromino) const;

    [[nodiscard]] bool isLineFull(int line) const;

    void clearSingleLine(int line);

    int clearFullLines();

    [[nodiscard]] tetroMat getBoardWithCurrentPiece() const;

    Tetromino* getCurrent() { return currentTetromino.has_value() ? &currentTetromino.value() : nullptr; }

    [[nodiscard]] const Tetromino* getCurrent() const { return currentTetromino.has_value() ? &currentTetromino.value() : nullptr; }

    void deleteCurrent() { currentTetromino.reset(); }

    [[nodiscard]] const tetroMat& getBoard() const { return board; }

    [[nodiscard]] tetroMat& getBoard() { return board; }

    [[nodiscard]] int getWidth() const  { return width; }

    [[nodiscard]] int getHeight() const { return height; }
};
