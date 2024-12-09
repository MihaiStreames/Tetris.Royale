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

    void generateBoardByDimension() { board = std::vector(height, std::vector(width, 0)); }

    [[nodiscard]] bool isColliding(const Tetromino& tetromino) const {
        const auto& shape = tetromino.getShape();
        const auto&[x, y] = tetromino.getPosition();

        for (int i = 0; i < static_cast<int>(shape.size()); ++i) {
            for (int j = 0; j < static_cast<int>(shape[i].size()); ++j) {
                if (shape[i][j] == 1) {
                    const int boardX = x + j;

                    if (const int boardY = y + i; boardX < 0 || boardX >= width || boardY < 0 || boardY >= height || board[boardY][boardX] == 1) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    bool trySpawnPiece(Tetromino piece) {
        if (!isColliding(piece)) {
            currentTetromino = piece;
            return true;
        }

        return false;
    }

    bool tryPlacePiece(const Tetromino& tetromino) {
        if (isColliding(tetromino)) return false;

        const auto& shape = tetromino.getShape();
        const auto&[x, y] = tetromino.getPosition();

        for (int i = 0; i < static_cast<int>(shape.size()); ++i) {
            for (int j = 0; j < static_cast<int>(shape[i].size()); ++j) {
                if (shape[i][j] == 1) {
                    const int boardX = x + j;
                    const int boardY = y + i;
                    board[boardY][boardX] = 1;
                }
            }
        }

        currentTetromino.reset();
        return true;
    }

    bool tryPlaceCurrentPiece() {
        if (!currentTetromino) return false;
        return tryPlacePiece(*currentTetromino);
    }

    [[nodiscard]] bool canMove(const Tetromino& tetromino, const int dx, const int dy) const {
        Tetromino moved = tetromino;
        const Position2D newPos = {moved.getPosition().x + dx, moved.getPosition().y + dy};
        moved.setPosition(newPos);
        return !isColliding(moved);
    }

    [[nodiscard]] bool tryMoveCurrent(const int dx, const int dy) {
        if (currentTetromino.has_value() && canMove(currentTetromino.value(), dx, dy)) {
            const Position2D newPos = {currentTetromino->getPosition().x + dx, currentTetromino->getPosition().y + dy};
            currentTetromino->setPosition(newPos);
            return true;
        }

        return false;
    }

    [[nodiscard]] bool canRotate(const Tetromino& tetromino, const bool clockwise) const {
        Tetromino rotated = tetromino;
        const auto shape = rotated.getRotateShape(clockwise ? RotateRight : RotateLeft);
        rotated.setShape(shape);
        return !isColliding(rotated);
    }

    [[nodiscard]] bool tryRotateCurrent(const bool clockwise) {
        if (currentTetromino.has_value() && canRotate(currentTetromino.value(), clockwise)) {
            const auto shape = currentTetromino->getRotateShape(clockwise ? RotateRight : RotateLeft);
            currentTetromino->setShape(shape);
            return true;
        }

        return false;
    }

    [[nodiscard]] bool tryMakeCurrentPieceFall() { return tryMoveCurrent(0, 1); }

    [[nodiscard]] int getRowsToObstacle(const Tetromino& tetromino) const {
        Tetromino temp = tetromino;
        int rowsToObstacle = 0;

        while (!isColliding(temp)) {
            Position2D newPos = {temp.getPosition().x, temp.getPosition().y + 1};
            temp.setPosition(newPos);
            ++rowsToObstacle;
        }

        return rowsToObstacle - 1;
    }

    [[nodiscard]] bool isLineFull(const int line) const {
        for (int x = 0; x < width; ++x) {
            if (board[line][x] == 0) return false;
        }

        return true;
    }

    void clearSingleLine(const int line) {
        for (int x = 0; x < width; ++x) {
            board[line][x] = 0;
        }

        for (int y = line; y > 0; --y) {
            board[y] = board[y - 1];
        }

        board[0] = std::vector(width, 0);
    }

    int clearFullLines() {
        int linesCleared = 0;

        for (int y = 0; y < height; ++y) {
            if (isLineFull(y)) {
                clearSingleLine(y);
                ++linesCleared;
            }
        }

        return linesCleared;
    }

    [[nodiscard]] tetroMat getBoardWithCurrentPiece() const {
        tetroMat ret = board;

        // If there's a current tetromino, overlay it onto the copied board
        if (currentTetromino.has_value()) {
            const Tetromino& piece = currentTetromino.value();
            const auto& shape = piece.getShape();
            const auto&[x, y] = piece.getPosition();
            const int pieceVal = static_cast<int>(piece.getPieceType()) + 1;

            for (int i = 0; i < static_cast<int>(shape.size()); ++i) {
                const int boardY = y + i;
                for (int j = 0; j < static_cast<int>(shape[i].size()); ++j) {
                    const int boardX = x + j;
                    if (shape[i][j] == 1) {
                        if (boardX >= 0 && boardX < width && boardY >= 0 && boardY < height) {
                            ret[boardY][boardX] = pieceVal;
                        }
                    }
                }
            }
        }

        return ret;
    }

    Tetromino* getCurrent() { return currentTetromino.has_value() ? &currentTetromino.value() : nullptr; }
    [[nodiscard]] const Tetromino* getCurrent() const { return currentTetromino.has_value() ? &currentTetromino.value() : nullptr; }
    void deleteCurrent() { currentTetromino.reset(); }

    [[nodiscard]] const tetroMat& getBoard() const { return board; }
    [[nodiscard]] int getWidth() const  { return width; }
    [[nodiscard]] int getHeight() const { return height; }
};