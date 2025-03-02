#include "classicEngine.hpp"
#include "classicGame.hpp"

void ClassicEngine::handleBasicPenalty(ClassicGame& game, const int linesCleared) {
    std::cout<<"in pen";
    if (linesCleared <= 1 ) return;

    const std::vector<ClassicGame*>& opponents = game.getOpponents();

    ClassicGame* opponent = game.getTarget();

    int linesToAdd;
    if (linesCleared < 4){
        linesToAdd = linesCleared -1;
    } else {
        linesToAdd = 4;
    }
    std::cout<<"shoudl get added";
    opponent->addPenaltyLines(linesToAdd);
}

void ClassicEngine::handleGameLogic(TetrisGame& game) {
    const int linesCleared = game.getGameMatrix().clearFullLines();
    handleScore(game, linesCleared);

    auto* classicGame = dynamic_cast<ClassicGame*>(&game);
    handleBasicPenalty(*classicGame, linesCleared);

    handleSpawn(game);  // ensure a piece is always available

    if (game.isGameOver()) handleGameOver(game);
}