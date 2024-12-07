#pragma once

#include <vector>
#include "common.hpp"

enum Rotation {
    LeftRotation,
    RightRotation
};

class Tetromino {
    Position2D position;
    std::vector<std::vector<int>> shape;
    PieceType pieceType;

public:
    Tetromino(Position2D startPos, PieceType type);
    Tetromino() : position({0,0}), shape(generateShapeByType(O)), pieceType(O) {}

    Position2D getPosition() const { return position; }
    PieceType getPieceType() const { return pieceType; }
    const std::vector<std::vector<int>>& getShape() const { return shape; }

    void setPieceType(PieceType newType);
    void setPosition(const Position2D& newPosition) { position = newPosition; }
    void setShape(const std::vector<std::vector<int>>& newShape) { shape = newShape; }

    Position2D getMovePosition(Action move) const;
    std::vector<std::vector<int>> getRotateShape(Rotation rotation) const;
    void reset();

protected:
    static std::vector<std::vector<int>> generateShapeByType(PieceType type);
};