#ifndef SERVER_RESPONSE_HPP
#define SERVER_RESPONSE_HPP

#include "Common.hpp"
#include "LobbyState.hpp"

#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

struct ServerResponseInterface
{
  public:
    virtual ~ServerResponseInterface() = default;
    virtual std::string serialize() const = 0;
};

struct ServerResponse : public ServerResponseInterface
{
    // This is a generic response struct. It will be used to send response to
    // the client. The response will contain the return status of the method
    // that was called, and the data that were generated server-side.

  public:
    int id;
    StatusCode status;
    std::unordered_map<std::string, std::string> data;

    [[nodiscard]] std::string serialize() const override;
    [[nodiscard]] static ServerResponse deserialize(const std::string& data);

    // Some automatic constructors for ServerResponse objects
    [[nodiscard]] static ServerResponse ErrorResponse(int id,
                                                      StatusCode status);
    [[nodiscard]] static ServerResponse
    ErrorResponse(int id, StatusCode status, const std::string& errorMessage);
    [[nodiscard]] static ServerResponse SuccessResponse(int id,
                                                        StatusCode status);
    [[nodiscard]] static ServerResponse
    SuccessResponse(int id, StatusCode status, const LobbyState& lobby);
    [[nodiscard]] static ServerResponse
    SuccessResponse(int id, StatusCode status, PlayerStatus playerStatus);
    [[nodiscard]] static ServerResponse
    SuccessResponse(int id, StatusCode status,
                    const std::unordered_map<std::string, std::string>& data);
};

#endif
