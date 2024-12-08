#pragma once

#include <vector>
#include "common.hpp"

class Tetromino {
    Position2D position;
    tetroMat shape;
    PieceType pieceType;

public:
    Tetromino(Position2D startPos, PieceType type);
    Tetromino() : position({0,0}), shape(generateShapeByType(O)), pieceType(O) {}

    [[nodiscard]] Position2D getPosition() const { return position; }
    [[nodiscard]] PieceType getPieceType() const { return pieceType; }
    [[nodiscard]] const tetroMat& getShape() const { return shape; }

    void setPieceType(PieceType newType);
    void setPosition(const Position2D& newPosition) { position = newPosition; }
    void setShape(const tetroMat& newShape) { shape = newShape; }

    [[nodiscard]] Position2D getMovePosition(Action move) const;
    [[nodiscard]] tetroMat getRotateShape(Action rotation) const;
    void reset();

protected:
    static tetroMat generateShapeByType(PieceType type);
};