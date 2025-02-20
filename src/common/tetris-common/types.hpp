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
    T,
    Single
};

enum Action {
    None,
    MoveLeft,
    MoveRight,
    MoveDown,
    RotateLeft,
    RotateRight,
    InstantFall,
    UseBag,
    Malus,
    Bonus
};

enum TypePowerUps {
    inverted_command,
    block_command,
    thunder_strike,
    fast_falling_pieces,
    light_off,
    
    blocs_1x1,
    slow_falling_pieces
};

using tetroMat = std::vector<std::vector<int>>;
using pieceVec = std::vector<PieceType>;