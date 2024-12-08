#pragma once

#include <vector>
#include <optional>
#include "types.hpp"

class Tetromino {
    Position2D position;
    tetroMat shape;
    PieceType pieceType;

public:
    Tetromino(const Position2D startPos, const PieceType type)
    : position(startPos), shape(generateShapeByType(type)), pieceType(type) {}

    Tetromino()
    : position({0,0}), shape(generateShapeByType(O)), pieceType(O) {}

    bool operator==(const Tetromino& other) const {
        return position.x == other.position.x &&
            position.y == other.position.y &&
                shape == other.shape;
    }

    [[nodiscard]] Position2D getPosition() const    { return position; }
    [[nodiscard]] PieceType getPieceType() const    { return pieceType; }
    [[nodiscard]] const tetroMat& getShape() const  { return shape; }

    void setPieceType(const PieceType newType) {
        pieceType = newType;
        shape = generateShapeByType(newType);
    }

    void setPosition(const Position2D& newPosition) { position = newPosition; }
    void setShape(const tetroMat& newShape)         { shape = newShape; }

    [[nodiscard]] Position2D getMovePosition(const Action move) const {
        Position2D newPos = position;

        switch (move) {
            case MoveLeft:  newPos.x -= 1; break;
            case MoveRight: newPos.x += 1; break;
            case MoveDown:  newPos.y += 1; break;
            default: break;
        }

        return newPos;
    }

    [[nodiscard]] tetroMat getRotateShape(const Action rotation) const {
        // If no rotation or invalid action for rotation, return current shape.
        if (rotation != RotateLeft && rotation != RotateRight) return shape;

        const int n = static_cast<int>(shape.size());
        if (n == 0) return shape;

        tetroMat rotatedShape(n, std::vector(n, 0));

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (rotation == RotateRight) {
                    rotatedShape[j][n - 1 - i] = shape[i][j];
                } else {
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

    [[nodiscard]] std::vector<Position2D> getAbsoluteCoordinates(
        const std::optional<Position2D> topLeft = std::nullopt,
        const std::optional<tetroMat> &shapeOverride = std::nullopt) const
    {
        auto [x, y] = topLeft.has_value() ? topLeft.value() : position;
        const tetroMat& usedShape = shapeOverride.has_value() ? shapeOverride.value() : shape;

        std::vector<Position2D> coords;
        for (int rel_y = 0; rel_y < static_cast<int>(usedShape.size()); ++rel_y) {
            for (int rel_x = 0; rel_x < static_cast<int>(usedShape[rel_y].size()); ++rel_x) {
                if (usedShape[rel_y][rel_x] != 0) {
                    const int abs_x = x + rel_x;
                    const int abs_y = y + rel_y;
                    coords.push_back(Position2D{abs_x, abs_y});
                }
            }
        }

        return coords;
    }

protected:
    static tetroMat generateShapeByType(const PieceType type) {
        switch (type) {
            case I: return {{0, 0, 0, 0},
                            {1, 1, 1, 1},
                            {0, 0, 0, 0},
                            {0, 0, 0, 0}};
            case O: return {{1, 1},
                            {1, 1}};
            case L: return {{0, 0, 1},
                            {1, 1, 1},
                            {0, 0, 0}};
            case S: return {{0, 1, 1},
                            {1, 1, 0},
                            {0, 0, 0}};
            case Z: return {{1, 1, 0},
                            {0, 1, 1},
                            {0, 0, 0}};
            case T: return {{0, 1, 0},
                            {1, 1, 1},
                            {0, 0, 0}};
            case J: return {{1, 0, 0},
                            {1, 1, 1},
                            {0, 0, 0}};
            default:
                // fallback shape if needed
                return {{1}};
        }
    }
};