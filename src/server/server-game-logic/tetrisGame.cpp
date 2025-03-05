#include "tetrisGame.hpp"

[[nodiscard]] bool TetrisGame::isGameOver() const { return gameOver; }
void TetrisGame::setGameOver(const bool flag) { gameOver = flag; }
void TetrisGame::setEnergy(int setEnergy) { energy = setEnergy; }
void TetrisGame::setBlockCommand(bool flag) { blockCommand = flag; }
void TetrisGame::setReverseControlTimeCount(int nbr) { reverseControlTimeCount += nbr; }
void TetrisGame::setActiveReverseControl(bool flag) { activeReverseControl = flag; }
void TetrisGame::setDarkMode(bool flag) { isDarkMode = flag; }
void TetrisGame::setDarkModeTimer(int time) { darkModeTimer = time; }

GameMatrix& TetrisGame::getGameMatrix() { return gameMatrix; }
int TetrisGame::getEnergy() { return energy; }
TetrisFactory& TetrisGame::getFactory() { return factory; }
Bag& TetrisGame::getBag()               { return bag; }

[[nodiscard]] const Tetromino *TetrisGame::getHoldPiece() const { return bag.peekPiece();}
[[nodiscard]] Tetromino &TetrisGame::getNextPiece(){ return factory.whatIsNextPiece();}

bool TetrisGame::getReverseControls() { return reverseControls; }
bool TetrisGame::getBlockCommand() { return blockCommand; }
int TetrisGame::getReverseControlTimeCount() { return reverseControlTimeCount; }
bool TetrisGame::getActiveReverseControl() { return activeReverseControl; }
bool TetrisGame::getDarkMode() { return isDarkMode; }
int TetrisGame::getDarkModeTimer() { return darkModeTimer; }

[[nodiscard]]  int TetrisGame::getFrameCount() const noexcept    { return frameCount; }
void TetrisGame::setFrameCount(const int fc)                    { frameCount = fc; }
void TetrisGame::incrementFrameCount(const int fc)            { frameCount += fc; }

[[nodiscard]]  int TetrisGame::getLevel() const noexcept { return level; }
void TetrisGame::setLevel(const int lvl)                { level = lvl; }
void TetrisGame::incrementLevel(const int lvl)        { level += lvl; }

[[nodiscard]]  int TetrisGame::getScore() const noexcept { return score; }

void TetrisGame::calculateScore(const int linesCleared) {
    // !! this sohuld be defined elsewhere, NOT in the cpp
    static const int tabScore[] = {0, 40, 100, 300, 1200};
    if (linesCleared >= 1 && linesCleared <= 4) score += tabScore[linesCleared];
}

void TetrisGame::calculateEnergy(const int linesCleared) {

    // !! this sohuld be defined elsewhere, NOT in the cpp
    // TODO : change the energy values (1 is giving 100 energy for debug)
    static const int tabEnergy[] = {0, 100, 30, 40, 60, 100};
    if (linesCleared >= 1 && linesCleared <= 5)
        energy += tabEnergy[linesCleared];

}

[[nodiscard]]  int TetrisGame::getLinesCleared() const               { return totalLinesCleared; }
void TetrisGame::setTotalLinesCleared(const int lines)              { totalLinesCleared = lines; }
void TetrisGame::incrementLinesCleared(const int q)                 { totalLinesCleared += q; }

[[nodiscard]]  bool TetrisGame::shouldApplyGravity() const {
    const int frame = getFrameCount();
    const int lvl = getLevel();
    int frame_quantum = 1;

    if (const auto it = SPEED_TABLE.find(lvl + speedFactor); it != SPEED_TABLE.end()) frame_quantum = it->second;

    return frame % frame_quantum == 0 && frame >= frame_quantum;
}

[[nodiscard]]  bool TetrisGame::shouldLevelUp() const { return getLinesCleared() / LINES_TO_LEVELUP != getLevel(); }

void TetrisGame::updateLevelAfterLineClear() {
    if (shouldLevelUp()) {
        const int newLineBasedLevel = getLinesCleared() / LINES_TO_LEVELUP;
        if (const int diff = newLineBasedLevel - getLevel(); diff > 0) incrementLevel(diff);
    }
}


// ? handle function ?

// Malus Method
void TetrisGame::inverted_command(bool flag) { std::cerr << "You tried to access power-ups via TetrisGame"; }

void TetrisGame::block_command() { std::cerr << "You tried to access power-ups via TetrisGame"; }

void TetrisGame::thunder_strike() { std::cerr << "You tried to access power-ups via TetrisGame"; }
void TetrisGame::fast_falling_pieces() { std::cerr << "You tried to access power-ups via TetrisGame"; }
void TetrisGame::darkMode() { std::cerr << "You tried to access power-ups via TetrisGame"; }
void TetrisGame::addPenaltyLines(int linesToAdd) { std::cerr << "You tried to access power-ups via TetrisGame"; } 

// Bonus Method
void TetrisGame::blocs_1x1() { std::cerr << "You tried to access power-ups via TetrisGame"; }
void TetrisGame::slow_falling_pieces() { std::cerr << "You tried to access power-ups via TetrisGame"; }