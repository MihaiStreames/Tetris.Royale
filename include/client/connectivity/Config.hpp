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
