#include "ServerResponse.hpp"


std::string ServerResponse::serialize() const {
    // serialize the response to a json string
    nlohmann::json j;

    j["id"] = id;
    j["status"] = status;
    j["data"] = data;

    return j.dump();
}


ServerResponse ServerResponse::deserialize(const std::string &data) {
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

    ServerResponse response;

    try {
        response.id = j["id"];
        response.status = j["status"];
        response.data = j["data"].get<std::unordered_map<std::string, std::string> >();
    } catch (nlohmann::json::exception &e) {
        throw std::runtime_error("[error] Unknown json error while deserializing Response: " + std::string(e.what()));
    } catch (std::exception &e) {
        throw std::runtime_error("[error] Unknown error while deserializing Response: " + std::string(e.what()));
    }

    return response;
}


ServerResponse ServerResponse::ErrorResponse(const int id, const StatusCode status) {
    // create an error response with the given id and status
    ServerResponse response;
    response.id = id;
    response.status = status;
    return response;
}

ServerResponse ServerResponse::ErrorResponse(const int id, const StatusCode status, const std::string &errorMessage) {
    // create an error response with the given id, status and error message
    ServerResponse response;
    response.id = id;
    response.status = status;
    response.data["error"] = errorMessage;
    return response;
}

ServerResponse ServerResponse::SuccessResponse(const int id, const StatusCode status) {
    // create a success response with the given id and status
    ServerResponse response;
    response.id = id;
    response.status = status;
    return response;
}

ServerResponse ServerResponse::SuccessResponse(const int id, const StatusCode status, const LobbyState &lobby) {
    // create a success response with the given id, status and lobby state

    std::string lobbyContent;

    try {
        // try to serialize the lobby state
        lobbyContent = lobby.serialize();
    } catch ([[maybe_unused]] std::runtime_error &e) {
        // error, we need to send an empty lobby state
        lobbyContent = LobbyState::generateEmptyState().serialize();
    }

    ServerResponse response;
    response.id = id;
    response.status = status;
    response.data["lobby"] = lobbyContent;

    return response;
}


ServerResponse ServerResponse::SuccessResponse(const int id, const StatusCode status, const PlayerStatus playerStatus) {
    // create a success response with the given id, status and player status
    ServerResponse response;
    response.id = id;
    response.status = status;
    response.data["status"] = std::to_string(static_cast<int>(playerStatus));
    return response;
}

ServerResponse ServerResponse::SuccessResponse(const int id, const StatusCode status,
                                               const std::unordered_map<std::string, std::string> &data) {
    // create a success response with the given id, status and data
    ServerResponse response;
    response.id = id;
    response.status = status;
    response.data = data;
    return response;
}
