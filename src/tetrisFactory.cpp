#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

#include "tetrisFactory.hpp"
#include "Tetromino.cpp"
#include "common.cpp"

using namespace std;

void TetrisFactory::fillPool() {
    if (pool.size() == 1) {
        Tetromino firstPiece = pool[0];
        vector<PieceType> newPool = {PieceType.I, PieceType.O, PieceType.T,
                                        PieceType.S, PieceType.Z, PieceType.J, PieceType.L}; // askip sa marche pas

        for (int i = 0; i < 7; i++) {
            if (newPool[i] == firstPiece) {
                newPool.erase(newPool.begin() + i);
                break;
            }
        }

        // shuffle
        random_device random;
        mt19937 rng(random());
        shuffle(newPool.begin(), newPool.end(), rng);

        newPool.push_back(firstPiece); // je sais pas si sa doit etre la derniere ou la premiere

        pool = newPool;
    }
}

void TetrisFactory::pushPiece(Tetromino tetromino) { pool.push_back(tetromino); }

Tetromino TetrisFactory::popPiece() { return pool.pop(); }

Tetromino TetrisFactory::whatIsNextPiece() { return pool[pool.size() - 1]; }

bool TetrisFactory::isPoolEmpty() {
    if (pool.size() > 0) {
        return false;
    } else {
        return true;
    }
}

int TetrisFactory::getPoolSize() {return pool.size(); }