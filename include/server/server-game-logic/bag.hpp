#pragma once

#include <stdexcept>
#include <memory>
#include "tetromino.hpp"

class Bag {
    std::unique_ptr<Tetromino> storedPiece;
    bool isUsable;

public:
    Bag() : storedPiece(nullptr), isUsable(true) {}

    Tetromino retrievePiece();

    [[nodiscard]] const Tetromino* peekPiece() const { return storedPiece.get(); }

    [[nodiscard]] bool isEmpty() const { return storedPiece == nullptr; }

    [[nodiscard]] bool usable() const { return isUsable; }

    void setUsable(const bool flag) { isUsable = flag; }

    // Stores a Tetromino in the bag if the bag is usable and empty.
    void storePiece(const Tetromino& piece);
    
};
