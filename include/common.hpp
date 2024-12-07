#pragma once

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
    InStandFall,
    UseBag
};