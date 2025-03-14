#include "LobbyState.hpp"

std::string
LobbyState::serialize() const {
    // serialize the lobby state to a JSON string
    nlohmann::json j;

    j["lobbyID"] = lobbyID;
    j["port"] = port;
    j["maxPlayers"] = maxPlayers;
    j["gameMode"] = gameMode;
    j["public"] = isPublic;

    j["players"] = players;
    j["readyPlayers"] = readyPlayers;
    j["spectators"] = spectators;

    return j.dump();
}

LobbyState
LobbyState::deserialize(const std::string &data) {
    // deserialize the JSON string to a lobby state
    nlohmann::json j;

    try {
        j = nlohmann::json::parse(data);
    } catch (nlohmann::json::parse_error &e) {
        throw std::runtime_error("JSON Error parsing LobbyState: " +
                                 std::string(e.what()));
    }
    catch (nlohmann::json::exception &e) {
        throw std::runtime_error("Unknown Error parsing LobbyState: " +
                                 std::string(e.what()));
    }

    LobbyState state;

    // see, I'm not lazy anymore :)
    try {
        state.lobbyID = j.at("lobbyID").get<std::string>();
        state.port = j.at("port").get<int>();
        state.maxPlayers = j.at("maxPlayers").get<int>();
        state.gameMode = static_cast<GameMode>(j.at("gameMode").get<int>());
        state.isPublic = j.at("public").get<bool>();
        state.players =
                j.at("players").get<std::unordered_map<std::string, std::string> >();
        state.readyPlayers =
                j.at("readyPlayers").get<std::unordered_map<std::string, bool> >();
        state.spectators =
                j.at("spectators")
                .get<std::unordered_map<std::string, std::string> >();
    } catch (nlohmann::json::type_error &e) {
        throw std::runtime_error("Type Error deserializing LobbyState: " +
                                 std::string(e.what()));
    }
    catch (nlohmann::json::out_of_range &e) {
        throw std::runtime_error("OutofRange Error deserializing LobbyState: " +
                                 std::string(e.what()));
    }
    catch (nlohmann::json::exception &e) {
        throw std::runtime_error("JSON Error deserializing LobbyState: " +
                                 std::string(e.what()));
    }

    return state;
}

LobbyState
LobbyState::generateEmptyState() {
    // generate an empty lobby state
    LobbyState state;
    state.lobbyID = "";
    state.port = EMPTY_LOBBY_PORT;
    state.maxPlayers = 0;
    state.gameMode = GameMode::NONE;
    state.isPublic = true;
    state.players = {};
    state.readyPlayers = {};
    state.spectators = {};

    return state;
}
