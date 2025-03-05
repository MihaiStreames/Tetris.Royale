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
    : currentTetromino(std::nullopt), width(wMatrix), height(hMatrix), board(hMatrix, std::vector(wMatrix, 0)) { }

    void generateBoardByDimension();

    [[nodiscard]] bool isColliding(const Tetromino& tetromino) const;

    bool trySpawnPiece(Tetromino piece);

    bool tryPlacePiece(const Tetromino& tetromino);

    bool tryPlaceCurrentPiece();

    [[nodiscard]] bool canMove(const Tetromino& tetromino, const int dx, const int dy) const;

    [[nodiscard]] bool tryMoveCurrent(const int dx, const int dy);

    [[nodiscard]] bool canRotate(const Tetromino& tetromino, const bool clockwise) const;

    [[nodiscard]] bool tryRotateCurrent(const bool clockwise);

    [[nodiscard]] bool tryMakeCurrentPieceFall();

    [[nodiscard]] int getRowsToObstacle(const Tetromino& tetromino) const;

    [[nodiscard]] bool isLineFull(const int line) const;

    void clearSingleLine(const int line);

    int clearFullLines();

    [[nodiscard]] tetroMat getBoardWithCurrentPiece() const;

    Tetromino* getCurrent();
    [[nodiscard]] const Tetromino* getCurrent() const;
    void deleteCurrent();

    [[nodiscard]] const tetroMat& getBoard() const;
    [[nodiscard]] tetroMat& getBoard();
    [[nodiscard]] int getWidth() const;
    [[nodiscard]] int getHeight() const;
};