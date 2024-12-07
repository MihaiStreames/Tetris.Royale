#include "gameMatrix.hpp"

GameMatrix::GameMatrix(const int wMatrix, const int hMatrix)
    : currentTetromino(nullptr), width(wMatrix), height(hMatrix), board(hMatrix, std::vector(wMatrix, 0)) {}

bool GameMatrix::isColliding(const Tetromino& tetromino) const {
    const auto& shape = tetromino.getShape();
    const auto&[x, y] = tetromino.getPosition();

    for (int i = 0; i < static_cast<int>(shape.size()); ++i) {
        for (int j = 0; j < static_cast<int>(shape[i].size()); ++j) {
            if (shape[i][j] == 1) {
                const int boardX = j + j;

                if (const int boardY = i + i; boardX < 0 || boardX >= width || boardY < 0 || boardY >= height || board[boardY][boardX] == 1) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool GameMatrix::trySpawnPiece(Tetromino& tetromino) {
    currentTetromino = &tetromino;
    if (!isColliding(tetromino)) return true;
    currentTetromino = nullptr;
    return false;
}

bool GameMatrix::tryPlacePiece(const Tetromino& tetromino) {
    const auto& shape = tetromino.getShape();
    const auto&[x, y] = tetromino.getPosition();

    for (int i = 0; i < static_cast<int>(shape.size()); ++i) {
        for (int j = 0; j < static_cast<int>(shape[i].size()); ++j) {
            if (shape[i][j] == 1) {
                const int boardX = j + j;
                const int boardY = i + i;
                board[boardY][boardX] = 1;
            }
        }
    }

    currentTetromino = nullptr;
    return true;
}

bool GameMatrix::canMove(const Tetromino& tetromino, const int dx, const int dy) const {
    Tetromino moved = tetromino;
    const Position2D newPos = {moved.getPosition().x + dx, moved.getPosition().y + dy};
    moved.setPosition(newPos);
    return !isColliding(moved);
}

bool GameMatrix::tryMoveCurrent(const int dx, const int dy) const {
    if (currentTetromino && canMove(*currentTetromino, dx, dy)) {
        const Position2D newPos = {currentTetromino->getPosition().x + dx, currentTetromino->getPosition().y + dy};
        currentTetromino->setPosition(newPos);
        return true;
    }

    return false;
}

bool GameMatrix::canRotate(const Tetromino& tetromino, const bool clockwise) const {
    Tetromino rotated = tetromino;
    const auto shape = rotated.getRotateShape(clockwise ? RotateRight : RotateLeft);
    rotated.setShape(shape);
    return !isColliding(rotated);
}

bool GameMatrix::tryRotateCurrent(const bool clockwise) const {
    if (currentTetromino && canRotate(*currentTetromino, clockwise)) {
        const auto shape = currentTetromino->getRotateShape(clockwise ? RotateRight : RotateLeft);
        currentTetromino->setShape(shape);
        return true;
    }
    return false;
}

int GameMatrix::getRowstoObstacle(const Tetromino& tetromino) const {
    Tetromino temp = tetromino;
    int rowsToObstacle = 0;

    while (!isColliding(temp)) {
        Position2D newPos = {temp.getPosition().x, temp.getPosition().y + 1};
        temp.setPosition(newPos);
        ++rowsToObstacle;
    }

    return rowsToObstacle - 1;
}

void GameMatrix::clearSingleLine(const int line) {
    for (int x = 0; x < width; ++x) {
        board[line][x] = 0;
    }

    for (int y = line; y > 0; --y) {
        board[y] = board[y - 1];
    }

    board[0] = std::vector(width, 0);
}

bool GameMatrix::isLineFull(const int line) const {
    for (int x = 0; x < width; ++x) {
        if (board[line][x] == 0) return false;
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