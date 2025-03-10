#pragma once

#include "Tetromino.hpp"

#include <memory>
#include <stdexcept>

class Bag
{
  private:
    std::unique_ptr<Tetromino> storedPiece;
    bool isUsable;

  public:
    Bag();
    ~Bag();

    // getters and setters stuff
    [[nodiscard]] bool isEmpty() const;
    [[nodiscard]] bool isBagUsable() const;
    void setUsable(const bool flag);

    // usage stuff
    [[nodiscard]] const Tetromino* peekPiece() const;
    [[nodiscard]] Tetromino retrievePiece();
    void storePiece(const Tetromino& piece);
};
