#pragma once

#include "Tetromino.hpp"
#include "Types.hpp"

#include <algorithm>
#include <random>
#include <vector>

class TetrisFactory {

  const pieceVec POSSIBLE_PIECES = {
    PieceType::I, PieceType::O, PieceType::L, PieceType::J,
    PieceType::Z, PieceType::S, PieceType::T
  };

public:

  TetrisFactory();
  ~TetrisFactory();

  void pushPiece(const Tetromino& tetromino);
  [[nodiscard]] Tetromino popPiece();
  [[nodiscard]] Tetromino& whatIsNextPiece();
  [[nodiscard]] bool isPoolEmpty() const;
  [[nodiscard]] int getPoolSize() const;


private:
    
  void fillPool();

  std::vector<Tetromino> pool;
  std::random_device rd;
  std::mt19937 rng;

};
