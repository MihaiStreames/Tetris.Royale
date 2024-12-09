#pragma once

#include <vector>

struct Position2D {
    int x;
    int y;
};

enum PieceType {
    O,
    L,
    J,
    I,
    Z,
    S,
    T
};

enum Action {
    None,
    MoveLeft,
    MoveRight,
    MoveDown,
    RotateLeft,
    RotateRight,
    InstantFall,
    UseBag
};

using tetroMat = std::vector<std::vector<int>>;
using pieceVec = std::vector<PieceType>;