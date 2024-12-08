#pragma once

#include <vector>
#include "types.hpp"

class Tetromino {
    Position2D position;
    tetroMat shape;
    PieceType pieceType;

public:
    Tetromino(Position2D startPos, PieceType type) : position(startPos), pieceType(type) {
        shape = generateShapeByType(pieceType);
    }
    Tetromino() : position({0,0}), shape(generateShapeByType(O)), pieceType(O) { }

    [[nodiscard]] Position2D getPosition() const { return position; }
    [[nodiscard]] PieceType getPieceType() const { return pieceType; }
    [[nodiscard]] const tetroMat& getShape() const { return shape; }

    void setPieceType(PieceType newType) {
        pieceType = newType;
        shape = generateShapeByType(newType);
    }

    void setPosition(const Position2D& newPosition) { position = newPosition; }
    void setShape(const tetroMat& newShape) { shape = newShape; }

    [[nodiscard]] Position2D getMovePosition(Action move) const {
        Position2D newPos = position;

        switch (move) {
            case MoveLeft: newPos.x -= 1; break;
            case MoveRight: newPos.x += 1; break;
            case MoveDown: newPos.y += 1; break;
            default: break;
        }

        return newPos;
    }

    [[nodiscard]] tetroMat getRotateShape(Action rotation) const {
        const int n = static_cast<int>(shape.size());
        std::vector rotatedShape(n, std::vector(n,0));

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (rotation == RotateRight) {
                    rotatedShape[j][n - 1 - i] = shape[i][j];
                }
                else if (rotation == RotateLeft) {
                    rotatedShape[n - 1 - j][i] = shape[i][j];
                }
            }
        }

        return rotatedShape;
    }

    void reset() {
        position = {0, 0};
        shape = generateShapeByType(pieceType);
    }

protected:
    static tetroMat generateShapeByType(PieceType type) {
        // Minimal example shapes
        switch (type) {
            case I: return {{1,1,1,1}};
            case O: return {{1,1},{1,1}};
            case T: return {{1,1,1},{0,1,0}};
            case S: return {{0,1,1},{1,1,0}};
            case Z: return {{1,1,0},{0,1,1}};
            case J: return {{1,0,0},{1,1,1}};
            case L: return {{0,0,1},{1,1,1}};
            default: return {{1}};
        }
    }
};