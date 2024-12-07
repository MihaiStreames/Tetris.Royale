#include "gameMatrix.hpp"

GameMatrix::GameMatrix(int width, int height)
    : width(width), height(height), board(height, std::vector<int>(width, 0)), currentTetromino(nullptr) {}

void GameMatrix::generateBoardDimension() {
    board = std::vector<std::vector<int>>(height, std::vector<int>(width, 0));
}

bool GameMatrix::isColliding(const Tetromino& Tetromino) const {
    const auto& shape = Tetromino.getShape();
    const auto& position = Tetromino.getPosition();

    for (int y = 0; y < shape.size(); ++y) {
        for (int x = 0; x < shape[y].size(); ++x) {
            if (shape[y][x] == 1) {
                int boardX = position.first + x;
                int boardY = position.second + y;

                if (boardX < 0 || boardX >= width || boardY < 0 || boardY >= height || board[boardY][boardX] == 1) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool GameMatrix::trySpawnPiece(Tetromino& Tetromino) {
    currentTetromino = &Tetromino;
    if (!isColliding(Tetromino)) {
        return true;
    }
    currentTetromino = nullptr;
    return false;
}

bool GameMatrix::tryPlacePiece(const Tetromino& Tetromino) {
    const auto& shape = Tetromino.getShape();
    const auto& position = Tetromino.getPosition();

    for (int y = 0; y < shape.size(); ++y) {
        for (int x = 0; x < shape[y].size(); ++x) {
            if (shape[y][x] == 1) {
                int boardX = position.first + x;
                int boardY = position.second + y;
                board[boardY][boardX] = 1;
            }
        }
    }

    currentTetromino = nullptr;
    return true;
}

bool GameMatrix::canMove(const Tetromino& Tetromino, int dx, int dy) const {
    Tetromino movedTetromino = Tetromino;
    movedTetromino.move({dx, dy});
    return !isColliding(movedTetromino);
}

bool GameMatrix::tryMoveCurrent(int dx, int dy) {
    if (currentTetromino && canMove(*currentTetromino, dx, dy)) {
        currentTetromino->move({dx, dy});
        return true;
    }
    return false;
}

bool GameMatrix::canRotate(const Tetromino& Tetromino, bool clockwise) const {
    Tetromino rotatedTetromino = Tetromino;
    rotatedTetromino.rotate(clockwise);
    return !isColliding(rotatedTetromino);
}

bool GameMatrix::tryRotateCurrent(bool clockwise) {
    if (currentTetromino && canRotate(*currentTetromino, clockwise)) {
        currentTetromino->rotate(clockwise);
        return true;
    }
    return false;
}

bool GameMatrix::tryMakeCurrentPieceFall() {
    return tryMoveCurrent(0, 1);
}

int GameMatrix::getRowstoObstacle(const Tetromino& Tetromino) const {
    Tetromino tempTetromino = Tetromino; 
    int rowsToObstacle = 0;            

    while (!isColliding(tempTetromino)) {
        tempTetromino.move({0, 1}); 
        ++rowsToObstacle;
    }

    return rowsToObstacle - 1; 
}

void GameMatrix::clearSingleLine(int line) {
    for (int x = 0; x < width; ++x) {
        board[line][x] = 0;
    }

    for (int y = line; y > 0; --y) {
        board[y] = board[y - 1];
    }

    board[0] = std::vector<int>(width, 0);
}

bool GameMatrix::isLineFull(int line) const {
    for (int x = 0; x < width; ++x) {
        if (board[line][x] == 0) {
            return false;
        }
    }
    return true;
}

int GameMatrix::clearFullLines() {
    int linesCleared = 0;

    for (int y = 0; y < height; ++y) {
        if (isLineFull(y)) {
            clearSingleLine(y);
            ++linesCleared;
        }
    }

    return linesCleared;
}

const std::vector<std::vector<int>>& GameMatrix::getBoard() const {
    return board;
}