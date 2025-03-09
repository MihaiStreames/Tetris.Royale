
#include "GameState.hpp"



std::string SpectatorState::serialize() const {

    // serialize the spectator state
    
    nlohmann::json j;
    j["playerUsername"] = playerUsername;
    j["playerGrid"] = playerGrid;
    j["nextTetro"] = nextTetro;
    j["holdTetro"] = holdTetro;
    return j.dump();

}

std::unique_ptr<SpectatorState> SpectatorState::deserialize(const std::string& data) {

    // deserialize the spectator state
    nlohmann::json j;

    try {
        j = nlohmann::json::parse(data);
    } catch (nlohmann::json::parse_error &e) {
        throw std::runtime_error(
            "[error] Parsing failed while deserializing SpectatorState: " + std::string(e.what()));
    } catch (nlohmann::json::exception &e) {
        throw std::runtime_error(
            "[error] Unknown json error while deserializing SpectatorState: " + std::string(e.what()));
    }

    std::unique_ptr<SpectatorState> state = std::make_unique<SpectatorState>();
    
    try {
        state->playerUsername = j["playerUsername"].get<std::string>();
        state->playerGrid = j["playerGrid"].get<tetroMat>();
        state->nextTetro = j["nextTetro"].get<PieceType>();
        state->holdTetro = j["holdTetro"].get<PieceType>();
    } catch (nlohmann::json::exception &e) {
        throw std::runtime_error(
            "[error] Unknown json error while deserializing SpectatorState: " + std::string(e.what()));
    } catch (std::exception &e) {
        throw std::runtime_error("[error] Unknown error while deserializing SpectatorState: " + std::string(e.what()));
    }

    return state;

}



std::string PlayerState::serialize() const {

    // serialize the player state
    nlohmann::json j;
    j["playerUsername"] = playerUsername;
    j["playerGrid"] = playerGrid;
    j["nextTetro"] = nextTetro;
    j["holdTetro"] = holdTetro;
    j["playerScore"] = playerScore;
    j["playerLevel"] = playerLevel;
    j["targetUsername"] = targetUsername;
    j["targetGrid"] = targetGrid;
    return j.dump();

}

std::unique_ptr<PlayerState> PlayerState::deserialize(const std::string& data) {

    // deserialize the player state
    nlohmann::json j;

    try {
        j = nlohmann::json::parse(data);
    } catch (nlohmann::json::parse_error &e) {
        throw std::runtime_error(
            "[error] Parsing failed while deserializing PlayerState: " + std::string(e.what()));
    } catch (nlohmann::json::exception &e) {
        throw std::runtime_error(
            "[error] Unknown json error while deserializing PlayerState: " + std::string(e.what()));
    }

    std::unique_ptr<PlayerState> state = std::make_unique<PlayerState>();
    
    try {
        state->playerUsername = j["playerUsername"].get<std::string>();
        state->playerGrid = j["playerGrid"].get<tetroMat>();
        state->nextTetro = j["nextTetro"].get<PieceType>();
        state->holdTetro = j["holdTetro"].get<PieceType>();
        state->playerScore = j["playerScore"].get<int>();
        state->playerLevel = j["playerLevel"].get<int>();
        state->targetUsername = j["targetUsername"].get<std::string>();
        state->targetGrid = j["targetGrid"].get<tetroMat>();
    } catch (nlohmann::json::exception &e) {
        throw std::runtime_error(
            "[error] Unknown json error while deserializing PlayerState: " + std::string(e.what()));
    } catch (std::exception &e) {
        throw std::runtime_error("[error] Unknown error while deserializing PlayerState: " + std::string(e.what()));
    }

    return state;

}



