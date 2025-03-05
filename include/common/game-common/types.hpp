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
    invertedControls,
    blockControls,
    thunderStrike,
    fastPieces,
    darkMode,
    singleBlocks,
    slowPieces
};

extern std::vector<TypePowerUps> malusVector;
extern std::vector<TypePowerUps> bonusVector;

// test avec inverseCommand : rajouter les autres malus une fois le reste implementé
// fast_falling_piece

using tetroMat = std::vector<std::vector<int>>;
using pieceVec = std::vector<PieceType>;