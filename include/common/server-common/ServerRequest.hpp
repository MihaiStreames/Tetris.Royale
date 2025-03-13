#ifndef SERVER_REQUEST_HPP
#define SERVER_REQUEST_HPP

#include "Common.hpp"

#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

struct ServerRequestInterface
{
  public:
    virtual ~ServerRequestInterface() = default;
    virtual std::string serialize() const = 0;
};

struct ServerRequest : public ServerRequestInterface
{
    // This is a generic request struct. It will be used to send requests to the
    // lobby server. The request will contain the method that was called, and
    // the parameters that were sent with the request. This is very similar
    // to RMI (Remote Method Invocation) in Java, I was just trying to make
    // this protocol myself, so I could learn more about networking, but
    // heck it's hard.

  public:
    int id;
    ServerMethods method;
    std::unordered_map<std::string, std::string> params;

    [[nodiscard]] std::string serialize() const override;
    [[nodiscard]] static ServerRequest deserialize(const std::string& data);
};

#endif
