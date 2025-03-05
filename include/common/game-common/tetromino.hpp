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

    bool operator==(const Tetromino& other) const;

    [[nodiscard]] Position2D getPosition() const    { return position; }

    [[nodiscard]] PieceType getPieceType() const    { return pieceType; }

    [[nodiscard]] const tetroMat& getShape() const  { return shape; }

    void setPieceType(PieceType newType);

    void setPosition(const Position2D& newPosition) { position = newPosition; }

    void setShape(const tetroMat& newShape)         { shape = newShape; }

    [[nodiscard]] Position2D getMovePosition(Action move) const;

    [[nodiscard]] tetroMat getRotateShape(Action rotation) const;

    void reset();

    [[nodiscard]] std::vector<Position2D> getAbsoluteCoordinates(
        std::optional<Position2D> topLeft = std::nullopt,
        const std::optional<tetroMat> &shapeOverride = std::nullopt) const;

protected:
    static tetroMat generateShapeByType(PieceType type);
};
