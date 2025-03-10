
#include "ClassicGame.hpp"

// #include "[[nodiscard]]lassicGame.hpp"  ->  funny copy paste error

ClassicGame::ClassicGame(const int gWidth, const int gHeight, const int gScore,
                         const int fc, const int lvl, const int totLinesCleared)
    : TetrisGame(gWidth, gHeight, gScore, fc, lvl, totLinesCleared)
{
    // this is the constructor for the ClassicGame class which is a subclass of
    // TetrisGame it initializes the game with the given parameters and
    // initializes the opponents vector and targetIndex (to 0)

    // ?? initialize opponents correctly or else this game will be boring (lol
    // what a great copilt auto comment)

    opponents = std::vector<TetrisGame*>();
    targetIndex = 0;
}

int
ClassicGame::getOpponentVectorSize()
{
    // returns the size of the opponents vector
    return static_cast<int>(opponents.size());
}

std::vector<TetrisGame*>
ClassicGame::getOpponents()
{
    // returns the opponents vector (vector of ClassicGame pointers)
    return opponents;
}

TetrisGame*
ClassicGame::getTarget()
{
    // returns the target ClassicGame pointer (targetIndex-th element of the
    // opponents vector) beware of out of bounds, so if that happens, returns
    // the first element of the opponents vector again, beware of the case where
    // the opponents vector is empty, so in that case, return nullptr

    int size = getOpponentVectorSize();

    if (size == 0)
    {
        return nullptr;
    }
    else if (targetIndex >= size)
    {
        return opponents[0]; // default
    }
    else
    {
        return opponents[targetIndex];
    }
}

int
ClassicGame::getTargetIndex()
{
    // returns the targetIndex
    return targetIndex;
}

void
ClassicGame::addOpponent(TetrisGame* opponent)
{
    // adds an opponent to the opponents vector
    opponents.push_back(opponent);
}

void
ClassicGame::removeOpponent(TetrisGame* opponent)
{
    // removes an opponent from the opponents vector
    // if the opponent is not found, then it does nothing

    for (int i = 0; i < getOpponentVectorSize(); i++)
    {
        if (opponents[i] == opponent)
        {
            opponents.erase(opponents.begin() + i);
            return;
        }
    }
}

void
ClassicGame::addPenaltyLines(int linesToAdd)
{
    // this method adds penalty lines to the game board
    // it first checks if the n-th top lines are empty, if not, then the game is
    // over if the game is not over, then it removes the top n lines and adds n
    // new lines at the bottom the new lines have a hole at a random position

    GameMatrix& matrix = getGameMatrix();

    // check if the top lines are empty (if not, game over)
    if (!matrix.areLinesEmpty(0, linesToAdd))
    {
        setGameOver(true);
        return;
    }

    // push the new lines into the board (at the bottom)
    matrix.pushPenaltyLinesAtBottom(linesToAdd);
}

void
ClassicGame::changePlayerView(int idx)
{
    // changes the targetIndex to the given index
    // go in a loop as a looping list

    if (idx < 0)
    {
        targetIndex = getOpponentVectorSize() - 1;
    }
    else if (idx >= getOpponentVectorSize())
    {
        targetIndex = 0;
    }
    else
    {
        targetIndex = idx;
    }
}
