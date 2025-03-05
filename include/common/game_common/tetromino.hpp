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
    bool operator==(const Tetromino& other) const;

    // Getters for the position, piece type, and shape of the tetromino.
    [[nodiscard]] Position2D getPosition() const;
    [[nodiscard]] PieceType getPieceType() const;
    [[nodiscard]] const tetroMat& getShape() const;

    // Sets the piece type and updates its shape accordingly.
    void setPieceType(const PieceType newType);

    // Updates the position of the tetromino.
    void setPosition(const Position2D& newPosition);

    // Updates the shape of the tetromino.
    void setShape(const tetroMat& newShape);

    // Calculates the new position based on a movement action (left, right, or down).
    [[nodiscard]] Position2D getMovePosition(const Action move) const;

    // Generates the rotated shape of the tetromino based on a rotation action (left or right).
    [[nodiscard]] tetroMat getRotateShape(const Action rotation) const;

    // Resets the tetromino to its default position (0, 0) and shape based on its type.
    void reset();

    // Returns the absolute coordinates of all blocks that make up the tetromino.
    // Allows overriding the top-left position or shape for calculations.
    [[nodiscard]] std::vector<Position2D> getAbsoluteCoordinates(
        const std::optional<Position2D> topLeft = std::nullopt,
        const std::optional<tetroMat> &shapeOverride = std::nullopt) const;

protected:
    // Generates the shape of a tetromino based on its type.
    static tetroMat generateShapeByType(const PieceType type);
};
