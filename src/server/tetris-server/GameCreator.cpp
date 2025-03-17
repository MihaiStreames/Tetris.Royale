#include "GameCreator.hpp"

const int GameCreator::GAME_WIDTH;
const int GameCreator::GAME_HEIGHT;

std::unordered_map<std::string, std::shared_ptr<TetrisGame> >
GameCreator::createGames(const GameMode &gameMode,
                         std::vector<std::string> &players) {
    // this helper function creates the games based on the game mode
    // note that util functions could be called directly to avoid the switch
    // statement, but that's easier to call this way

    switch (gameMode) {
        case GameMode::CLASSIC:
            return createClassicGames(players);
        case GameMode::DUEL:
            return createClassicGames(players);
        case GameMode::ROYALE:
            return createRoyaleGames(players);

        case GameMode::NONE:
            throw std::invalid_argument("[err] Invalid game mode");
        default:
            throw std::invalid_argument("[err] Invalid argument");
    }
}

std::unordered_map<std::string, std::shared_ptr<TetrisGame> >
GameCreator::createClassicGames(std::vector<std::string> &players) {
    // create a map of classic games with the player token as the key

    std::unordered_map<std::string, std::shared_ptr<TetrisGame>> games;

    for (auto &player: players) {
        games[player] =
                std::make_shared<ClassicGame>(GAME_WIDTH, GAME_HEIGHT, 0, 0, 0, 0, player);
    }

    // initialize the opponents for each player
    for (auto &player: players) {
        for (auto &opponent: players) {
            if (player != opponent) {
                games[player]->addOpponent(games[opponent].get());
            }
        }
    }

    return games;
}

std::unordered_map<std::string, std::shared_ptr<TetrisGame> >
GameCreator::createRoyaleGames(std::vector<std::string> &players) {
    // create a map of royal games with the player token as the key

    std::unordered_map<std::string, std::shared_ptr<TetrisGame> > games;

    for (auto &player: players) {
        games[player] =
                std::make_shared<RoyalGame>(GAME_WIDTH, GAME_HEIGHT, 0, 0, 0, 0, player);
    }

    // initialize the opponents for each player
    for (auto &player: players) {
        for (auto &opponent: players) {
            if (player != opponent) {
                games[player]->addOpponent(games[opponent].get());
            }
        }
    }

    return games;
}

std::shared_ptr<GameEngine>
GameCreator::createEngine(const GameMode &gameMode) {
    // this helper function creates the engine based on the game mode
    // note that util functions could be called directly to avoid the switch
    // statement, but that's easier to call this way

    switch (gameMode) {
        case GameMode::CLASSIC:
            return createClassicEngine();
        case GameMode::DUEL:
            return createClassicEngine();
        case GameMode::ROYALE:
            return createRoyalEngine();

        case GameMode::NONE:
            throw std::invalid_argument("[err] Invalid game mode");
        default:
            throw std::invalid_argument("[err] Invalid argument");
    }
}

std::shared_ptr<GameEngine>
GameCreator::createClassicEngine() {
    // create a classic engine

    return std::make_shared<ClassicEngine>();
}

std::shared_ptr<GameEngine>
GameCreator::createRoyalEngine() {
    // create a royal engine

    return std::make_shared<RoyalEngine>();
}
