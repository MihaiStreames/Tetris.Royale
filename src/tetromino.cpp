#include "tetromino.hpp"

std::vector<std::vector<int>> Tetromino::generateShapeByType(const PieceType type) {
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

Tetromino::Tetromino(const Position2D startPos, const PieceType type)
    : position(startPos), pieceType(type) {
    shape = generateShapeByType(pieceType);
}

void Tetromino::setPieceType(const PieceType newType) {
    pieceType = newType;
    shape = generateShapeByType(newType);
}

void Tetromino::reset() {
    position = {0, 0};
    shape = generateShapeByType(pieceType);
}

Position2D Tetromino::getMovePosition(const Action move) const {
    Position2D newPos = position;

    switch (move) {
        case MoveLeft: newPos.x -= 1; break;
        case MoveRight: newPos.x += 1; break;
        case MoveDown: newPos.y += 1; break;
        default: break;
    }

    return newPos;
}

std::vector<std::vector<int>> Tetromino::getRotateShape(const Rotation rotation) const {
    const int n = static_cast<int>(shape.size());
    std::vector rotatedShape(n, std::vector(n,0));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (rotation == RightRotation) {
                rotatedShape[j][n - 1 - i] = shape[i][j];
            }
            else if (rotation == LeftRotation) {
                rotatedShape[n - 1 - j][i] = shape[i][j];
            }
        }
    }

    return rotatedShape;
}
