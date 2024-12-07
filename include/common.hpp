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
    Move,
    Rotation,
    InStandFall,
    UseBag,
    NULL
};

enum Move {
    Left,
    Right,
    Down,
    NULL
};

enum Rotation {
    Left,
    Right,
    NULL
};