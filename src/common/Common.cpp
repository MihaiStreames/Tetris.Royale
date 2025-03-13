#include "Common.hpp"

std::string
getStatusCodeString(StatusCode code) {
    // return the string representation of the status code
    // this is used for debugging purposes

    switch (code) {
        case StatusCode::SUCCESS:
            return "SUCCESS";
        case StatusCode::SUCCESS_REPLACED_SESSION:
            return "SUCCESS_REPLACED_SESSION";
        case StatusCode::ERROR:
            return "ERROR";
        case StatusCode::ERROR_CREATING_SOCKET:
            return "ERROR_CREATING_SOCKET";
        case StatusCode::ERROR_BINDING_SOCKET:
            return "ERROR_BINDING_SOCKET";
        case StatusCode::ERROR_SETTING_SOCKET_OPTIONS:
            return "ERROR_SETTING_SOCKET_OPTIONS";
        case StatusCode::ERROR_NO_AVAILABLE_PORT:
            return "ERROR_NO_AVAILABLE_PORT";
        case StatusCode::ERROR_INITIALIZING_SOCKET:
            return "ERROR_INITIALIZING_SOCKET";
        case StatusCode::ERROR_NOT_CONNECTED:
            return "ERROR_NOT_CONNECTED";
        case StatusCode::ERROR_SENDING_REQUEST:
            return "ERROR_SENDING_REQUEST";
        case StatusCode::ERROR_RECEIVING_RESPONSE:
            return "ERROR_RECEIVING_RESPONSE";
        case StatusCode::ERROR_INVALID_PORT:
            return "ERROR_INVALID_PORT";
        case StatusCode::ERROR_RESTORING_PORT:
            return "ERROR_RESTORING_PORT";
        case StatusCode::ERROR_CHANGING_PORT:
            return "ERROR_CHANGING_PORT";
        case StatusCode::ERROR_CLIENT_NOT_IN_LOBBY:
            return "ERROR_CLIENT_NOT_IN_LOBBY";
        case StatusCode::ERROR_CLIENT_ALREADY_IN_LOBBY:
            return "ERROR_CLIENT_ALREADY_IN_LOBBY";
        case StatusCode::ERROR_LOBBY_FULL:
            return "ERROR_LOBBY_FULL";
        case StatusCode::ERROR_LOBBY_NOT_FOUND:
            return "ERROR_LOBBY_NOT_FOUND";
        case StatusCode::ERROR_INVALID_LOBBY_SIZE:
            return "ERROR_INVALID_LOBBY_SIZE";
        case StatusCode::ERROR_MAX_PLAYERS_REACHED:
            return "ERROR_MAX_PLAYERS_REACHED";
        case StatusCode::ERROR_MAX_LOBBIES_REACHED:
            return "ERROR_MAX_LOBBIES_REACHED";
        case StatusCode::ERROR_CREATING_GAMES:
            return "ERROR_CREATING_GAMES";
        case StatusCode::ERROR_CREATING_ENGINE:
            return "ERROR_CREATING_ENGINE";
        case StatusCode::ERROR_GETTING_GAME_STATE:
            return "ERROR_GETTING_GAME_STATE";
        case StatusCode::ERROR_DESERIALIZING_REQUEST:
            return "ERROR_DESERIALIZING_REQUEST";
        case StatusCode::ERROR_NOT_IMPLEMENTED:
            return "ERROR_NOT_IMPLEMENTED";
        case StatusCode::ERROR_UNKNOWN_METHOD:
            return "ERROR_UNKNOWN_METHOD";
        case StatusCode::ERROR_SESSION_ALREADY_EXISTS:
            return "ERROR_SESSION_ALREADY_EXISTS";
        case StatusCode::ERROR_SESSION_NOT_FOUND:
            return "ERROR_SESSION_NOT_FOUND";
        case StatusCode::ERROR_NO_SESSION_TOKEN:
            return "ERROR_NO_SESSION_TOKEN";
        case StatusCode::ERROR_SPECTATOR_CANNOT_READY:
            return "ERROR_SPECTATOR_CANNOT_READY";
        case StatusCode::ERROR_PLAYER_ALREADY_READY:
            return "ERROR_PLAYER_ALREADY_READY";
        case StatusCode::ERROR_PLAYER_NOT_READY:
            return "ERROR_PLAYER_NOT_READY";
        case StatusCode::ERROR_SPECTATOR_CANNOT_INTERACT:
            return "ERROR_SPECTATOR_CANNOT_INTERACT";
        case StatusCode::ERROR_INVALID_TOKEN:
            return "ERROR_INVALID_TOKEN";
        case StatusCode::ERROR_NOT_SUPPOSED_TO_HAPPEN:
            return "ERROR_NOT_SUPPOSED_TO_HAPPEN";
        default:
            return "UNKNOWN_STATUS_CODE";
    }
}

std::string
getServerMethodString(const ServerMethods method) {
    switch (method) {
        case ServerMethods::GET_CURRENT_LOBBY:
            return "GET_CURRENT_LOBBY";
        case ServerMethods::LEAVE_LOBBY:
            return "LEAVE_LOBBY";
        case ServerMethods::READY:
            return "READY";
        case ServerMethods::UNREADY:
            return "UNREADY";
        case ServerMethods::GET_GAME_STATE:
            return "GET_GAME_STATE";
        case ServerMethods::KEY_STROKE:
            return "KEY_STROKE";
        case ServerMethods::START_SESSION:
            return "START_SESSION";
        case ServerMethods::END_SESSION:
            return "END_SESSION";
        case ServerMethods::GET_LOBBY:
            return "GET_LOBBY";
        case ServerMethods::GET_PUBLIC_LOBBIES:
            return "GET_PUBLIC_LOBBIES";
        case ServerMethods::CREATE_LOBBY:
            return "CREATE_LOBBY";
        case ServerMethods::JOIN_LOBBY:
            return "JOIN_LOBBY";
        case ServerMethods::SPECTATE_LOBBY:
            return "SPECTATE_LOBBY";
        case ServerMethods::NONE:
            return "NONE";
        default:
            return "UNKNOWN_METHOD";
    }
}
