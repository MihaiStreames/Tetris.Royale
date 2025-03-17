#include "GameState.hpp"

SpectatorState
SpectatorState::generateEmptyState() {
    // generate an empty spectator state
    SpectatorState state;
    state.playerUsername = "";
    state.playerGrid = tetroMat();
    state.nextTetro = PieceType::None;
    state.holdTetro = PieceType::None;
    return state;
}

std::string
SpectatorState::serialize() const {
    // serialize the spectator state

    nlohmann::json j;
    j["playerUsername"] = playerUsername;
    j["playerGrid"] = playerGrid;
    j["nextTetro"] = nextTetro;
    j["holdTetro"] = holdTetro;
    return j.dump();
}

SpectatorState
SpectatorState::deserialize(const std::string &data) {
    // deserialize the spectator state
    nlohmann::json j;

    try {
        j = nlohmann::json::parse(data);
    } catch (nlohmann::json::parse_error &e) {
        throw std::runtime_error(
            "[error] Parsing failed while deserializing SpectatorState: " +
            std::string(e.what()));
    }
    catch (nlohmann::json::exception &e) {
        throw std::runtime_error(
            "[error] Unknown json error while deserializing SpectatorState: " +
            std::string(e.what()));
    }

    SpectatorState state;

    try {
        state.playerUsername = j["playerUsername"].get<std::string>();
        state.playerGrid = j["playerGrid"].get<tetroMat>();
        state.nextTetro = j["nextTetro"].get<PieceType>();
        state.holdTetro = j["holdTetro"].get<PieceType>();
    } catch (nlohmann::json::exception &e) {
        throw std::runtime_error(
            "[error] Unknown json error while deserializing SpectatorState: " +
            std::string(e.what()));
    }
    catch (std::exception &e) {
        throw std::runtime_error(
            "[error] Unknown error while deserializing SpectatorState: " +
            std::string(e.what()));
    }

    return state;
}

PlayerState
PlayerState::generateEmptyState() {
    // generate an empty player state
    PlayerState state;
    state.playerUsername = "";
    state.playerGrid = tetroMat();
    state.nextTetro = PieceType::None;
    state.holdTetro = PieceType::None;
    state.playerScore = 0;
    state.playerLevel = 0;
    state.playerLines = 0;
    state.playerEnergy = 0;
    state.targetUsername = "";
    state.targetGrid = tetroMat();
    return state;
}

std::string
PlayerState::serialize() const {
    // serialize the player state
    nlohmann::json j;
    j["playerUsername"] = playerUsername;
    j["playerGrid"] = playerGrid;
    j["nextTetro"] = nextTetro;
    j["holdTetro"] = holdTetro;
    j["playerScore"] = playerScore;
    j["playerLevel"] = playerLevel;
    j["playerLines"] = playerLines;
    j["playerEnergy"] = playerEnergy;
    j["targetUsername"] = targetUsername;
    j["targetGrid"] = targetGrid;
    return j.dump();
}

PlayerState
PlayerState::deserialize(const std::string &data) {
    // deserialize the player state
    nlohmann::json j;

    try {
        j = nlohmann::json::parse(data);
    } catch (nlohmann::json::parse_error &e) {
        throw std::runtime_error(
            "[error] Parsing failed while deserializing PlayerState: " +
            std::string(e.what()));
    }
    catch (nlohmann::json::exception &e) {
        throw std::runtime_error(
            "[error] Unknown json error while deserializing PlayerState: " +
            std::string(e.what()));
    }

    PlayerState state;

    try {
        state.playerUsername = j["playerUsername"].get<std::string>();
        state.playerGrid = j["playerGrid"].get<tetroMat>();
        state.nextTetro = j["nextTetro"].get<PieceType>();
        state.holdTetro = j["holdTetro"].get<PieceType>();
        state.playerScore = j["playerScore"].get<int>();
        state.playerLevel = j["playerLevel"].get<int>();
        state.playerLines = j["playerLines"].get<int>();
        state.playerEnergy = j["playerEnergy"].get<int>();
        state.targetUsername = j["targetUsername"].get<std::string>();
        state.targetGrid = j["targetGrid"].get<tetroMat>();
    } catch (nlohmann::json::exception &e) {
        throw std::runtime_error(
            "[error] Unknown json error while deserializing PlayerState: " +
            std::string(e.what()));
    }
    catch (std::exception &e) {
        throw std::runtime_error(
            "[error] Unknown error while deserializing PlayerState: " +
            std::string(e.what()));
    }

    return state;
}
