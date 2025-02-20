#pragma once

#include <vector>
#include <optional>
#include "types.hpp"

// The Tetromino class represents a single tetromino piece in a Tetris game.
// It tracks the piece's type, position, and shape, and provides methods for movement, rotation, and resetting.
class Tetromino {
    Position2D position; // Current position of the tetromino on the game board.
    tetroMat shape; // Matrix representing the shape of the tetromino.
    PieceType pieceType; // Type of the tetromino (e.g., I, O, T, etc.).

public:
    // Constructor: Initializes a tetromino with a specific type and starting position.
    Tetromino(const Position2D startPos, const PieceType type)
    : position(startPos), shape(generateShapeByType(type)), pieceType(type) {}

    // Default constructor: Creates a default O-type tetromino at position (0, 0).
    Tetromino()
    : position({0,0}), shape(generateShapeByType(O)), pieceType(O) {}

    // Equality operator: Checks if two tetrominos have the same position and shape.
    bool operator==(const Tetromino& other) const {
        return position.x == other.position.x &&
            position.y == other.position.y &&
                shape == other.shape;
    }

    // Getters for the position, piece type, and shape of the tetromino.
    [[nodiscard]] Position2D getPosition() const    { return position; }
    [[nodiscard]] PieceType getPieceType() const    { return pieceType; }
    [[nodiscard]] const tetroMat& getShape() const  { return shape; }

    // Sets the piece type and updates its shape accordingly.
    void setPieceType(const PieceType newType) {
        pieceType = newType;
        shape = generateShapeByType(newType);
    }

    // Updates the position of the tetromino.
    void setPosition(const Position2D& newPosition) { position = newPosition; }

    // Updates the shape of the tetromino.
    void setShape(const tetroMat& newShape)         { shape = newShape; }

    // Calculates the new position based on a movement action (left, right, or down).
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

    // Generates the rotated shape of the tetromino based on a rotation action (left or right).
    [[nodiscard]] tetroMat getRotateShape(const Action rotation) const {
        // If no rotation or invalid action for rotation, return current shape.
        if (rotation != RotateLeft && rotation != RotateRight) return shape;

        const int n = static_cast<int>(shape.size());
        if (n == 0) return shape;

        tetroMat rotatedShape(n, std::vector(n, 0)); // Create a matrix for the rotated shape.

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (rotation == RotateRight) { // Clockwise rotation.
                    rotatedShape[j][n - 1 - i] = shape[i][j];
                } else { // Counterclockwise rotation.
                    rotatedShape[n - 1 - j][i] = shape[i][j];
                }
            }
        }

        return rotatedShape;
    }

    // Resets the tetromino to its default position (0, 0) and shape based on its type.
    void reset() {
        position = {0, 0};
        shape = generateShapeByType(pieceType);
    }

    // Returns the absolute coordinates of all blocks that make up the tetromino.
    // Allows overriding the top-left position or shape for calculations.
    [[nodiscard]] std::vector<Position2D> getAbsoluteCoordinates(
        const std::optional<Position2D> topLeft = std::nullopt,
        const std::optional<tetroMat> &shapeOverride = std::nullopt) const
    {
        auto [x, y] = topLeft.has_value() ? topLeft.value() : position; // Use provided top-left position if available.
        const tetroMat& usedShape = shapeOverride.has_value() ? shapeOverride.value() : shape; // Use provided shape if available.

        std::vector<Position2D> coords;
        for (int rel_y = 0; rel_y < static_cast<int>(usedShape.size()); ++rel_y) {
            for (int rel_x = 0; rel_x < static_cast<int>(usedShape[rel_y].size()); ++rel_x) {
                if (usedShape[rel_y][rel_x] != 0) { // Check if the cell is part of the tetromino.
                    const int abs_x = x + rel_x; // Calculate absolute x-coordinate.
                    const int abs_y = y + rel_y; // Calculate absolute y-coordinate.
                    coords.push_back(Position2D{abs_x, abs_y}); // Add the coordinate to the list.
                }
            }
        }

        return coords; // Return the list of absolute coordinates.
    }

protected:
    // Generates the shape of a tetromino based on its type.
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
            case Single: return {{1}};
            default:
                // Fallback shape if needed.
                return {{1}};
        }
    }
};
