#pragma once

#include <vector>

struct Position2D
{
    int x;
    int y;
};

enum class PieceType
{
    None,
    O,
    L,
    J,
    I,
    Z,
    S,
    T,
    Single,
};

enum class Action
{
    None,
    MoveLeft,
    MoveRight,
    MoveDown,
    RotateLeft,
    RotateRight,
    InstantFall,
    UseBag,
    UseMalus,
    UseBonus,
    SeePreviousOpponent,
    SeeNextOpponent,
};

enum class TypePowerUps
{
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

using tetroMat = std::vector<std::vector<int>>;
using pieceVec = std::vector<PieceType>;
using tetroShape = std::vector<std::vector<bool>>;
