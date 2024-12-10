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
    Tetromino retrievePiece() {
        if (!isEmpty()) { // Check if the bag contains a piece.
            Tetromino piece = *storedPiece; // Copy the stored piece.
            storedPiece.reset(); // Clear the stored piece from the bag.
            return piece; // Return the retrieved Tetromino.
        }

        throw std::runtime_error("Bag is empty."); // Throw an error if the bag is empty.
    }

    // Returns a const pointer to the stored Tetromino, or nullptr if the bag is empty.
    [[nodiscard]] const Tetromino* peekPiece() const { return storedPiece.get(); }

    // Checks if the bag is currently empty.
    [[nodiscard]] bool isEmpty() const { return storedPiece == nullptr; }

    // Checks if the bag is usable for storing or swapping Tetrominos.
    [[nodiscard]] bool usable() const { return isUsable; }

    // Sets the usability flag for the bag.
    void setUsable(const bool flag) { isUsable = flag; }

    // Stores a Tetromino in the bag if the bag is usable and empty.
    void storePiece(const Tetromino& piece) {
        if (isUsable && isEmpty()) { // Ensure the bag is usable and empty.
            storedPiece = std::make_unique<Tetromino>(piece); // Store a copy of the Tetromino.
            storedPiece->reset(); // Reset the Tetromino to its default state.
        }
    }
};
