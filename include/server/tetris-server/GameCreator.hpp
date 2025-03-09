#ifndef GAME_CREATOR_HPP
#define GAME_CREATOR_HPP


#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <iostream>

#include "common.hpp"

#include "tetrisGame.hpp"
#include "royalGame.hpp"
#include "classicGame.hpp"

#include "gameEngine.hpp"
#include "classicEngine.hpp"
#include "royalEngine.hpp"



class GameCreator {

    const static int GAME_WIDTH = 10;
    const static int GAME_HEIGHT = 22;


public:

    // game creator stuff
    static std::unordered_map<std::string, std::shared_ptr<TetrisGame>> createGames(const GameMode& gameMode, std::vector<std::string>& players);
    static std::unordered_map<std::string, std::shared_ptr<TetrisGame>> createClassicGames(std::vector<std::string>& players);
    static std::unordered_map<std::string, std::shared_ptr<TetrisGame>> createRoyaleGames(std::vector<std::string>& players);

    // engine creator stuff
    static std::shared_ptr<GameEngine> createEngine(const GameMode& gameMode);
    static std::shared_ptr<GameEngine> createClassicEngine();
    static std::shared_ptr<GameEngine> createRoyalEngine();


};


#endif

