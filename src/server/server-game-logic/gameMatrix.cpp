#include "gameMatrix.hpp"

[[nodiscard]] bool GameMatrix::isColliding(const Tetromino& tetromino) const {
    const tetroMat& shape = tetromino.getShape();
    const auto&[x, y] = tetromino.getPosition();

    for (int i = 0; i < static_cast<int>(shape.size()); ++i) {
        for (int j = 0; j < static_cast<int>(shape[i].size()); ++j) {
            if (shape[i][j] == 1) {
                const int boardX = x + j;

                if (const int boardY = y + i; boardX < 0 || boardX >= width || boardY < 0 || boardY >= height || board[boardY][boardX] != 0) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool GameMatrix::trySpawnPiece(Tetromino piece) {
    if (!isColliding(piece)) {
        currentTetromino = piece;
        return true;
    }

    return false;
}

bool GameMatrix::tryPlacePiece(const Tetromino& tetromino) {
    if (isColliding(tetromino)) return false;

    const tetroMat& shape = tetromino.getShape();
    const auto&[x, y] = tetromino.getPosition();

    for (int i = 0; i < static_cast<int>(shape.size()); ++i) {
        for (int j = 0; j < static_cast<int>(shape[i].size()); ++j) {
            if (shape[i][j] == 1) {
                const int boardX = x + j;
                const int boardY = y + i;
                board[boardY][boardX] = static_cast<int>(tetromino.getPieceType()) + 1;
            }
        }
    }

    currentTetromino.reset();
    return true;
}

bool GameMatrix::tryPlaceCurrentPiece() {
    if (!currentTetromino) return false;
    return tryPlacePiece(*currentTetromino);
}

[[nodiscard]] bool GameMatrix::canMove(const Tetromino& tetromino, const int dx, const int dy) const {
    Tetromino moved = tetromino;
    const Position2D newPos = {moved.getPosition().x + dx, moved.getPosition().y + dy};
    moved.setPosition(newPos);
    return !isColliding(moved);
}

[[nodiscard]] bool GameMatrix::tryMoveCurrent(const int dx, const int dy) {
    if (currentTetromino.has_value() && canMove(currentTetromino.value(), dx, dy)) {
        const Position2D newPos = {currentTetromino->getPosition().x + dx, currentTetromino->getPosition().y + dy};
        currentTetromino->setPosition(newPos);
        return true;
    }

    return false;
}

bool GameMatrix::tryInstantFall() {
    
    bool moved = false;
    while (tryMoveDown()) {
        moved = true;
    }

    return moved;

}

[[nodiscard]] bool GameMatrix::canRotate(const Tetromino& tetromino, const bool clockwise) const {
    Tetromino rotated = tetromino;
    const tetroMat shape = rotated.getRotateShape(clockwise ? RotateRight : RotateLeft);
    rotated.setShape(shape);
    return !isColliding(rotated);
}

[[nodiscard]] bool GameMatrix::tryRotateCurrent(const bool clockwise) {
    if (currentTetromino.has_value() && canRotate(currentTetromino.value(), clockwise)) {
        const tetroMat shape = currentTetromino->getRotateShape(clockwise ? RotateRight : RotateLeft);
        currentTetromino->setShape(shape);
        return true;
    }

    return false;
}

[[nodiscard]] int GameMatrix::getRowsToObstacle(const Tetromino& tetromino) const {
    Tetromino temp = tetromino;
    int rowsToObstacle = 0;

    while (!isColliding(temp)) {
        Position2D newPos = {temp.getPosition().x, temp.getPosition().y + 1};
        temp.setPosition(newPos);
        ++rowsToObstacle;
    }

    return rowsToObstacle - 1;
}

[[nodiscard]] bool GameMatrix::isLineFull(const int line) const {
    for (int x = 0; x < width; ++x) {
        if (board[line][x] == 0) return false;
    }

    return true;
}

[[nodiscard]] bool GameMatrix::isLineEmpty(const int line) const {
    for (int x = 0; x < width; ++x) {
        if (board[line][x] != 0) return false;
    }

    return true;
}

[[nodiscard]] bool GameMatrix::areLinesEmpty(const int start, const int end) const {
    for (int y = start; y < end; ++y) {
        if (!isLineEmpty(y)) return false;
    }

    return true;
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

void GameMatrix::pushNewLinesAtBottom(const std::vector<std::vector<int>> newLines) {
    board.erase(board.begin(), board.begin() + newLines.size());
    for (const auto& line : newLines) {
        board.push_back(line);
    }
}

void GameMatrix::pushPenaltyLinesAtBottom(const int linesToAdd) {
    

    std::vector<std::vector<int>> newLines;

    for (int i = 0; i < linesToAdd; ++i) {
        std::vector<int> line(width, Single);
        const int hole = rand() % width;
        line[hole] = 0;
        newLines.push_back(line);
    }

    pushNewLinesAtBottom(newLines);
}

[[nodiscard]] tetroMat GameMatrix::getBoardWithCurrentPiece() const {
    tetroMat ret = board;

    // If there's a current tetromino, overlay it onto the copied board
    if (currentTetromino.has_value()) {
        const Tetromino& piece = currentTetromino.value();
        const tetroMat& shape = piece.getShape();
        const auto&[x, y] = piece.getPosition();
        const int pieceVal = static_cast<int>(piece.getPieceType()) + 1;

        for (int i = 0; i < static_cast<int>(shape.size()); ++i) {
            const int boardY = y + i;
            for (int j = 0; j < static_cast<int>(shape[i].size()); ++j) {
                const int boardX = x + j;
                if (shape[i][j] == 1) {
                    if (boardX >= 0 && boardX < width && boardY >= 0 && boardY < height) {
                        ret[boardY][boardX] = pieceVal;
                    }
                }
            }
        }
    }

    return ret;
}
