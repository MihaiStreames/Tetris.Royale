#include "classicEngine.hpp"

void ClassicEngine::sendToEnemy(int linesCleared, TetrisGame& enemyGame) {
    enemyGame.addPenaltyLines(linesCleared);
}