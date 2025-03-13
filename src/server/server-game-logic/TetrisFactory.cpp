#include "TetrisFactory.hpp"

void
TetrisFactory::fillPool()
{
    // if pool's empty, then refill it with pieces

    if (pool.empty())
    {
        pieceVec newPool = possiblePieces;
        // hinter does whatever idk dont mind
        std::ranges::shuffle(newPool, rng);

        pool.clear();
        for (auto p : newPool)
        {
            pool.emplace_back(Position2D{0, 0}, p);
        }
    }
}

void
TetrisFactory::pushPiece(const Tetromino& tetromino)
{
    pool.push_back(tetromino);
}

Tetromino
TetrisFactory::popPiece()
{
    if (pool.empty())
    {
        fillPool();
    }

    Tetromino piece = pool.back();
    pool.pop_back();

    return piece;
}

Tetromino&
TetrisFactory::whatIsNextPiece()
{
    if (pool.empty())
    {
        fillPool();
    }
    return pool.back();
}

bool
TetrisFactory::isPoolEmpty() const
{
    return pool.empty();
}
int
TetrisFactory::getPoolSize() const
{
    return static_cast<int>(pool.size());
}
