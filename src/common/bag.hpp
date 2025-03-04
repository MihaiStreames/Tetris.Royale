#pragma once

#include <stdexcept>
#include <memory>
#include "tetromino.hpp"

// The Bag class manages a single Tetromino that can be stored and retrieved later.
// It includes functionality to check if the bag is usable, store a Tetromino,
// retrieve it, or peek at the stored piece without modifying it.
class Bag {
    std::unique_ptr<Tetromino> storedPiece; // Stores the current Tetromino, or nullptr if the bag is empty.
    bool isUsable; // Indicates whether the bag can be used to store or swap pieces.

public:
    // Constructor: Initializes the bag as empty and usable.
    Bag() : storedPiece(nullptr), isUsable(true) {}

    // Retrieves the stored Tetromino, resetting the bag to empty.
    // Throws an exception if the bag is empty.
    Tetromino retrievePiece();

    // Returns a const pointer to the stored Tetromino, or nullptr if the bag is empty.
    [[nodiscard]] const Tetromino* peekPiece() const;

    [[nodiscard]] bool isEmpty() const;

    // Checks if the bag is usable for storing or swapping Tetrominos.
    [[nodiscard]] bool usable() const;

    // Sets the usability flag for the bag.
    void setUsable(const bool flag);

    // Stores a Tetromino in the bag if the bag is usable and empty.
    void storePiece(const Tetromino& piece);
};
