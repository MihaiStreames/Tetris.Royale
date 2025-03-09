#ifndef SERVER_RESPONSE_HPP
#define SERVER_RESPONSE_HPP

#include <iostream>
#include <unordered_map>
#include <string>
#include <nlohmann/json.hpp>

#include "common.hpp"
#include "LobbyState.hpp"


struct ServerResponseInterface {

    public:

        virtual ~ServerResponseInterface() = default;
        virtual std::string serialize() const = 0;

};


struct ServerResponse : public ServerResponseInterface {

    // This is a generic response struct. It will be used to send response to the
    // client. The response will contain the return status of the method that was called, and
    // the data that were generated server-side.

    public:

        int id;
        StatusCode status;
        std::unordered_map<std::string, std::string> data;
        
        [[nodiscard]] std::string serialize() const override;
        [[nodiscard]] static ServerResponse deserialize(const std::string& data);

        // Some automatic constructors for ServerResponse objects
        [[nodiscard]] static ServerResponse ErrorResponse(int id, StatusCode status);
        [[nodiscard]] static ServerResponse ErrorResponse(int id, StatusCode status, const std::string& errorMessage);
        [[nodiscard]] static ServerResponse SuccessResponse(int id, StatusCode status);
        [[nodiscard]] static ServerResponse SuccessResponse(int id, StatusCode status, const LobbyState &lobby);
        [[nodiscard]] static ServerResponse SuccessResponse(int id, StatusCode status, PlayerStatus playerStatus);
        [[nodiscard]] static ServerResponse SuccessResponse(int id, StatusCode status, const std::unordered_map<std::string, std::string>& data);

};

#endif
