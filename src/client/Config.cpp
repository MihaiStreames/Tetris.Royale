#include "Config.hpp"

Config::Config(const std::string& filename) : filename(filename)
{

    // constructor for config class
    // Initialize default configuration values

    defaultConfig = {
        {"server_ip", MASTER_SERVER_IP},
        {"lobby_port", std::to_string(LOBBY_SERVER_PORT)},
        {"db_port", std::to_string(DB_SERVER_PORT)},
    };
}

Config::~Config()
{

    // destructor for config class
    // save the configuration file if it's open

    std::ofstream configFile(filename);
    if (configFile.is_open())
    {
        nlohmann::json configJson(configData);
        configFile << configJson.dump(INDENT_SIZE_CONFIG);
    }
    else
    {
        std::cerr << "[err] Unable to save config file." << std::endl;
    }
}

void
Config::load()
{

    // load the configuration file
    // if the file is open, read the json data and store it in the configData
    // map otherwise, generate the default configuration

    std::ifstream configFile(filename);

    // if the file is open, read the json data and store it in the configData
    // map
    if (configFile.is_open())
    {

        nlohmann::json configJson;
        configFile >> configJson;
        configData = configJson.get<std::map<std::string, std::string>>();

        // otherwise, generate the default configuration
    }
    else
    {
        generateDefaultConfig();
    }
}

std::string
Config::get(const std::string& key)
{

    // get the value of a key from the configData map
    // if the key is found, return the value

    auto it = configData.find(key);
    if (it != configData.end())
    {
        return it->second;
    }

    throw std::invalid_argument("[err] Key not found in config file.");

    return ""; // this is to make the compiler shut the fuck up lol
}

void
Config::generateDefaultConfig()
{

    // generate the default configuration
    // open the file and write the default configuration to it
    // store the default configuration in the configData map

    std::ofstream configFile(filename);

    // open the file and write the default configuration to it
    if (configFile.is_open())
    {

        nlohmann::json defaultJson(defaultConfig);
        configFile << defaultJson.dump(INDENT_SIZE_CONFIG);
        configData = defaultConfig;

        // store the default configuration in the configData map
    }
    else
    {
        std::cerr << "Unable to create default config file." << std::endl;
    }
}
