#include "tetromino.hpp"

bool Tetromino::operator==(const Tetromino& other) const {
    return position.x == other.position.x &&
        position.y == other.position.y &&
            shape == other.shape;
}

void Tetromino::setPieceType(const PieceType newType) {
    pieceType = newType;
    shape = generateShapeByType(newType);
}

[[nodiscard]] Position2D Tetromino::getMovePosition(const Action move) const {
    Position2D newPos = position;

    switch (move) {
        case MoveLeft:  newPos.x -= 1; break;
        case MoveRight: newPos.x += 1; break;
        case MoveDown:  newPos.y += 1; break;
        default: break;
    }

    return newPos;
}

[[nodiscard]] tetroMat Tetromino::getRotateShape(const Action rotation) const {
    // If no rotation or invalid action for rotation, return current shape
    if (rotation != RotateLeft && rotation != RotateRight) return shape;

    const int n = static_cast<int>(shape.size());
    if (n == 0) return shape;

    tetroMat rotatedShape(n, std::vector(n, 0)); // Create a matrix for the rotated shape

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

// Resets the tetromino to its default position (0, 0) and shape based on its type
void Tetromino::reset() {
    position = {0, 0};
    shape = generateShapeByType(pieceType);
}

[[nodiscard]] std::vector<Position2D> Tetromino::getAbsoluteCoordinates(
    const std::optional<Position2D> topLeft,
    const std::optional<tetroMat> &shapeOverride) const
{
    auto [x, y] = topLeft.has_value() ? topLeft.value() : position;
    const tetroMat& usedShape = shapeOverride.has_value() ? shapeOverride.value() : shape;

    std::vector<Position2D> coords;
    for (int rel_y = 0; rel_y < static_cast<int>(usedShape.size()); ++rel_y) {
        for (int rel_x = 0; rel_x < static_cast<int>(usedShape[rel_y].size()); ++rel_x) {
            if (usedShape[rel_y][rel_x] != 0) { // Check if the cell is part of the tetromino
                const int abs_x = x + rel_x;
                const int abs_y = y + rel_y;
                coords.push_back(Position2D{abs_x, abs_y}); // Add the coordinate to the list
            }
        }
    }

    return coords;
}

tetroMat Tetromino::generateShapeByType(const PieceType type) {
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
            return {{1}};
    }
}