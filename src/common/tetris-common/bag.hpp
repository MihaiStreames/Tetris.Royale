#pragma once

#include <stdexcept>
#include <memory>
#include "tetromino.hpp"

class Bag {
    std::unique_ptr<Tetromino> storedPiece;
    bool isUsable;

public:
    Bag() : storedPiece(nullptr), isUsable(true) {}

    Tetromino retrievePiece() {
        if (!isEmpty()) {
            Tetromino piece = *storedPiece;
            storedPiece.reset();
            return piece;
        }

        throw std::runtime_error("Bag is empty.");
    }

    [[nodiscard]] const Tetromino* peekPiece() const { return storedPiece.get(); }

    [[nodiscard]] bool isEmpty() const { return storedPiece == nullptr; }
    [[nodiscard]] bool usable() const { return isUsable; }
    void setUsable(const bool flag) { isUsable = flag; }

    void storePiece(const Tetromino& piece) {
        if (isUsable && isEmpty()) {
            storedPiece = std::make_unique<Tetromino>(piece);
        }
    }
};