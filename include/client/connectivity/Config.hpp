#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

#include "Common.hpp"
#include <nlohmann/json.hpp>

class Config
{

  std::unordered_map<std::string, std::string> DEFAULT_CONFIG = {
    // connection settings
    {"server_ip", MASTER_SERVER_IP},
    {"lobby_port", std::to_string(LOBBY_SERVER_PORT)},
    {"db_port", std::to_string(DB_SERVER_PORT)},
    // keybinds
    {actionToString(Action::MoveDown), "s"},
    {actionToString(Action::MoveLeft), "a"},
    {actionToString(Action::MoveRight), "d"},
    {actionToString(Action::RotateLeft), "q"},
    {actionToString(Action::RotateRight), "e"},
    {actionToString(Action::InstantFall), "v"},
    {actionToString(Action::UseBag), "c"},
    {actionToString(Action::UseMalus), "m"},
    {actionToString(Action::UseBonus), "b"},
    {actionToString(Action::SeePreviousOpponent), "w"},
    {actionToString(Action::SeeNextOpponent), "s"},
  };

  public:
    Config(const std::string& filename);
    ~Config();

    void load();
    [[nodiscard]] std::string get(const std::string& key);

  private:
    void generateDefaultConfig();

    std::string filename;
    std::map<std::string, std::string> configData;
    nlohmann::json defaultConfig;
};

#endif
