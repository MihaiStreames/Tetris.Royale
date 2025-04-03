#ifndef GAME_CREATOR_HPP
#define GAME_CREATOR_HPP

#include "ClassicEngine.hpp"
#include "ClassicGame.hpp"
#include "Common.hpp"
#include "GameEngine.hpp"
#include "RoyalEngine.hpp"
#include "RoyalGame.hpp"
#include "TetrisGame.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class GameCreator
{
    const static int GAME_WIDTH = 10;
    const static int GAME_HEIGHT = 22;

  public:
    // game creator stuff
    static std::unordered_map<std::string, std::shared_ptr<TetrisGame>>
    createGames(const GameMode& gameMode, std::vector<std::string>& players);

    static std::unordered_map<std::string, std::shared_ptr<TetrisGame>>
    createClassicGames(std::vector<std::string>& players);

    static std::unordered_map<std::string, std::shared_ptr<TetrisGame>>
    createEndlessGame(std::vector<std::string>& players);

    static std::unordered_map<std::string, std::shared_ptr<TetrisGame>>
    createRoyaleGames(std::vector<std::string>& players);

    // engine creator stuff
    static std::shared_ptr<GameEngine> createEngine(const GameMode& gameMode);

    static std::shared_ptr<GameEngine> createClassicEngine();

    static std::shared_ptr<GameEngine> createRoyalEngine();
};

#endif
