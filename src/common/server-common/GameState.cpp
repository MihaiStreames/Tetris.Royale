#include "GameState.hpp"


// TODO : Create some PlayerState class with the player's bag and current piece
//        and all the spectator shit as well


std::string SpectatorState::serialize() const {
    nlohmann::json j;
    j["sessionToken"] = sessionToken;
    j["playerGrids"] = playerGrids;
    j["scores"] = scores;

    return j.dump();
}

SpectatorState SpectatorState::deserialize(const std::string &data) {
    // deserialize the response from a json string
    nlohmann::json j;

    // some try-catch blocks because I'm not lazy anymore (I'm still lazy)
    try {
        j = nlohmann::json::parse(data);
    } catch (nlohmann::json::parse_error &e) {
        throw std::runtime_error("[error] Parsing failed while deserializing Response: " + std::string(e.what()));
    } catch (nlohmann::json::exception &e) {
        throw std::runtime_error("[error] Unknown json error while deserializing Response: " + std::string(e.what()));
    }

    SpectatorState state;

    try {
        state.sessionToken = j["sessionToken"].get<std::string>();
        state.playerGrids = j["playerGrids"].get<std::unordered_map<std::string, Matrix > >();
        state.scores = j["scores"].get<std::unordered_map<std::string, int> >();
    } catch (nlohmann::json::exception &e) {
        throw std::runtime_error("[error] Unknown json error while deserializing Response: " + std::string(e.what()));
    } catch (std::exception &e) {
        throw std::runtime_error("[error] Unknown error while deserializing Response: " + std::string(e.what()));
    }

    return state;
}

SpectatorState SpectatorState::generateEmptyState() {
    SpectatorState state;
    state.sessionToken = "";
    state.playerGrids = {};
    state.scores = {};

    return state;
}
