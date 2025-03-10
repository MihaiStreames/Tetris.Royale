
#include "GameMatrix.hpp"




GameMatrix::GameMatrix(const int wMatrix, const int hMatrix)
    : width(wMatrix), height(hMatrix), board(generateBoardByDimension(wMatrix, hMatrix)) {
    
        // this is the constructor of the GameMatrix class
        // it initializes the board with the dimensions given in the constructor

        // more stuff might be added here in the future, but for now this is all that is needed


}




const Tetromino* GameMatrix::getCurrent() const {
    return currentTetromino.has_value() ? &currentTetromino.value() : nullptr;
}

int GameMatrix::getWidth() const {
    return width;
}

int GameMatrix::getHeight() const {
    return height;
}

tetroMat& GameMatrix::getBoard() {
    return board;
}

tetroMat GameMatrix::getBoardWithCurrentPiece() const {

    // this method is used to get the board with the current piece on it
    // it is used to render the board with the current piece on it

    tetroMat boardWithCurrentPiece = board;
    const Tetromino* currentTetromino = getCurrent();

    // check if there is a current piece
    if (!currentTetromino) { return boardWithCurrentPiece; }

    // if there is a current piece, get its shape, shape size and position
    const tetroShape& shape = currentTetromino->getShape();
    const auto&[x, y] = currentTetromino->getPosition();
    int n = static_cast<int>(shape.size());

    // iterate over the shape of the current piece and add it to the board
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {

            // lol would you believe : there was a random +1 here which would cause the piece
            // to have a different color when on the board (what the fuck even happened here, does
            // someone even read this code lol?)

            if (shape[i][j]) {

                int nx = x + j;
                int ny = y + i;

                // check if the indices are within the bounds of the board
                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    boardWithCurrentPiece[ny][nx] = static_cast<int>(currentTetromino->getPieceType());
                }

            }

        }
    }

    return boardWithCurrentPiece;

}



void GameMatrix::setCurrent(const Tetromino &tetromino) {
    deleteCurrent();
    currentTetromino.emplace(tetromino);
}



bool GameMatrix::canMove(const Tetromino& tetromino, const int dx, const int dy) {
    
    // this method is used to check if a piece, given a move (dx, dy), is colliding with
    // something on the board

    const Position2D newPos = {tetromino.getPosition().x + dx, tetromino.getPosition().y + dy};
    Tetromino moved = Tetromino(newPos, tetromino.getPieceType(), tetromino.getShape());

    return !isColliding(moved);
    
}

bool GameMatrix::tryMoveCurrent(const int dx, const int dy) {

    // this method is used to try move the current piece, given a move (dx, dy).
    // will return true if the move was sucessful, false otherwise

    const Tetromino* current = getCurrent();

    // if no current tetro
    if (!current) { return false; }

    // if the move is not possible
    if (!canMove(*current, dx, dy)) { return false; }

    // if the move is possible, move the piece
    Position2D newPos = {current->getPosition().x + dx, current->getPosition().y + dy};
    currentTetromino->setPosition(newPos);

    return true;

}

bool GameMatrix::tryMoveLeft() {
    return tryMoveCurrent(-1, 0);
}

bool GameMatrix::tryMoveRight() {
    return tryMoveCurrent(1, 0);
}

bool GameMatrix::tryMoveDown() {
    return tryMoveCurrent(0, 1);
}

bool GameMatrix::tryInstantFall() {
    
    bool moved = false;
    while (tryMoveDown()) {
        moved = true;
    }

    return moved;

}




bool GameMatrix::canRotate(const Tetromino& tetromino, const bool clockwise) {
    
    // this method is used to check if a piece, given a rotation (clockwise or not),
    // is colliding with something on the board

    const tetroShape rotatedShape = tetromino.getRotateShape(clockwise ? Action::RotateLeft : Action::RotateRight);
    Tetromino rotatedTetro = Tetromino(tetromino.getPosition(), tetromino.getPieceType(), rotatedShape);

    return !isColliding(rotatedTetro);

}

bool GameMatrix::tryRotateCurrent(bool clockwise) {
    
    // this method is used to try rotate the current piece, given a rotation (clockwise or not).
    // will return true if the rotation was sucessful, false otherwise

    const Tetromino* current = getCurrent();

    // if no current tetro
    if (!current) { return false; }

    // if the rotation is not possible
    if (!canRotate(*current, clockwise)) { return false; }

    // if the rotation is possible, rotate the piece
    tetroShape rotatedShape = current->getRotateShape(clockwise ? Action::RotateLeft : Action::RotateRight);
    currentTetromino->setShape(rotatedShape);

    return true;

}

bool GameMatrix::tryRotateLeft() {
    return tryRotateCurrent(true);
}

bool GameMatrix::tryRotateRight() {
    return tryRotateCurrent(false);
}



bool GameMatrix::tryMakeCurrentPieceFall() {
    
    // alias for tryMoveDown, used in the game engines.
    // not really sure why this was made, it's been too long since I wrote the python
    // version this is based on

    return tryMoveDown();

}

bool GameMatrix::tryPlacePiece(const Tetromino& tetromino) {

    // this method is used to try place a piece on the board
    // will return true if the piece was placed, false otherwise

    if (isColliding(tetromino)) { return false; }

    // ?? ok so I know this code is extremely similar to the one in getBoardWithCurrentPiece
    // ?? and we could theoretically refactor this to use the same code, but I'm not sure
    // ?? if it's worth my time rn. I'll leave it like this for now, but please if you're reading this
    // ?? and you're working on this code, consider refactoring this

    // if the piece is not colliding, place it on the board
    // get the shape, shape size and position of the piece
    const tetroShape& shape = tetromino.getShape();
    int n = static_cast<int>(shape.size());
    const auto&[x, y] = tetromino.getPosition();
    
    // iterate over the shape of the piece and add it to the board
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {

            if (shape[i][j]) {
                board[y + i][x + j] = static_cast<int>(tetromino.getPieceType());
            }

        }
    }

    currentTetromino.reset();
    return true;

}

bool GameMatrix::tryPlaceCurrentPiece() {

    // this method is used to try place the current piece on the board
    // will return true if the piece was placed, false otherwise

    const Tetromino* current = getCurrent();

    // if no current tetro
    if (!current) { return false; }

    return tryPlacePiece(*current);

}

bool GameMatrix::trySpawnPiece(Tetromino piece) {
    
    // this method is used to try spawn a piece on the board
    // will return true if the piece was spawned, false otherwise

    if (isColliding(piece)) { return false; }

    setCurrent(piece);
    return true;

}

void GameMatrix::deleteCurrent() {
    currentTetromino.reset();
}





bool GameMatrix::isTileEmpty(const int x, const int y) {
    
    // this method is used to check if a tile on the board is empty
    // it returns true if the tile is empty, false otherwise

    return getBoard()[y][x] == static_cast<int>(PieceType::None);

}

bool GameMatrix::isColliding(const Tetromino& tetromino) {

    // this method is used to check if a piece is colliding with something on the board
    // it returns true if the piece is colliding, false otherwise
    
    const tetroShape& shape = tetromino.getShape();
    int n = static_cast<int>(shape.size());
    const auto&[x, y] = tetromino.getPosition();

    // iterate over the shape of the piece and check if it is colliding with something on the board
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {

            // !! this has been changed without testing, please check if it works

            if (shape[i][j]) {
                const int nx = x + j;
                const int ny = y + i;

                if (nx < 0 || nx >= width || ny >= height) { return true; }
                if (ny < 0) { continue; }
                if (!isTileEmpty(nx, ny)) { return true; }

            }

        }
    }

    return false;

}

bool GameMatrix::isLineFull(const int line) {
    
    // this method is used to check if a line is full
    // it returns true if the line is full, false otherwise

    for (int x = 0; x < getWidth(); ++x) {
        if (isTileEmpty(x, line)) { return false; }
    }

    return true;

}

bool GameMatrix::isLineEmpty(const int line) {
    
    // this method is used to check if a line is empty
    // it returns true if the line is empty, false otherwise

    for (int x = 0; x < getWidth(); ++x) {
        if (!isTileEmpty(x, line)) { return false; }
    }

    return true;

}

bool GameMatrix::areLinesEmpty(const int start, const int end) {
    
    // this method is used to check if a range of lines is empty
    // it returns true if the range of lines is empty, false otherwise

    for (int y = start; y < end; ++y) {
        if (!isLineEmpty(y)) return false;
    }

    return true;

}

int GameMatrix::getRowsToObstacle(const Tetromino& tetromino) {

    // this method is used to get the number of rows to the obstacle
    // it returns the number of rows to the obstacle

    // ?? I know this is ugly, but honestly it will do for now
    // ?? This is most likely not going to be refactored, but if you're reading this,
    // ?? feel free to do so

    Tetromino temp = tetromino;
    int rowsToObstacle = 0;

    while (!isColliding(temp)) {
        Position2D newPos = {temp.getPosition().x, temp.getPosition().y + 1};
        temp.setPosition(newPos);
        ++rowsToObstacle;
    }

    return rowsToObstacle - 1;

}

int GameMatrix::findHighestBlockInColumn(const int col) {
    
    // this method is used to find the highest block in a column
    // it returns the y position of the highest block in the column or -1 if the column is empty
    // used to calculate the impact position of the thunder strike powerup

    int y = -1;
    
    for (int i = 0; i < getHeight(); ++i) {
        if (!isTileEmpty(col, i)) {
            y = i;
            break;
        }
    }

    return y;

}





void GameMatrix::clearSingleLine(const int line) {

    // this method is used to clear a single line
    // it sets all the tiles in the line to empty and moves the lines above it down
    // this is the core of the game logic, as this is what happens when a line is cleared

    // TODO: access to the board is direct, this needs to be changed to use the getter

    // set all the tiles in the line to empty
    for (int x = 0; x < getWidth(); ++x) {
        board[line][x] = static_cast<int>(PieceType::None);
    }

    // move the lines above it down by one
    for (int y = line; y > 0; --y) {
        board[y] = board[y - 1];
    }

    // set the top line to empty
    board[0] = std::vector(getWidth(), static_cast<int>(PieceType::None));

}

int GameMatrix::clearFullLines() {

    // this method is used to clear all the full lines
    // it returns the number of lines cleared

    int linesCleared = 0;

    for (int y = 0; y < getHeight(); ++y) {
        if (isLineFull(y)) {
            clearSingleLine(y);
            ++linesCleared;
        }
    }

    return linesCleared;
    
}

void GameMatrix::pushNewLinesAtBottom(const std::vector<std::vector<int>> newLines) {
    
    // this method is used to push new lines at the bottom of the board
    // it is used to add new lines to the board when a player is hit by a penalty

    // TODO: access to the board is direct, this needs to be changed to use the getter

    board.erase(board.begin(), board.begin() + newLines.size());
    for (const auto& line : newLines) {
        board.push_back(line);
    }

}

void GameMatrix::pushPenaltyLinesAtBottom(const int linesToAdd) {
    
    // this method is used to push penalty lines at the bottom of the board
    // it is used to add penalty lines to the board when a player is hit by a penalty

    std::vector<std::vector<int>> newLines;

    for (int i = 0; i < linesToAdd; ++i) {
        std::vector<int> line(getWidth(), static_cast<int>(PieceType::Single));
        const int hole = rand() % getWidth();
        line[hole] = static_cast<int>(PieceType::None);
        newLines.push_back(line);
    }

    pushNewLinesAtBottom(newLines);

}

void GameMatrix::destroyAreaAroundBlock(const Position2D pos, const int blastRadius) {
    
    // this method is used to destroy an area around a block
    // it is used to destroy an area around a block when a player is hit by a thunder strike

    // TODO: access to the board is direct, this needs to be changed to use the getter

    const int x = pos.x;
    const int y = pos.y;

    for (int dy = -blastRadius; dy <= blastRadius; ++dy) {
        for (int dx = -blastRadius; dx <= blastRadius; ++dx) {

            const int nx = x + dx;
            const int ny = y + dy;

            if (nx >= 0 && nx < getWidth() && ny >= 0 && ny < getHeight()) {
                board[ny][nx] = static_cast<int>(PieceType::None);
            }

        }
    }
    
}



tetroMat GameMatrix::generateBoardByDimension(int width, int height) {

    // this method is used to generate the board with the dimensions given in the constructor
    // it is used in the constructor to initialize the board with the correct dimensions

    return tetroMat(height, std::vector<int>(width, static_cast<int>(PieceType::None)));

}

