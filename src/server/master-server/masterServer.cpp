#include "masterServer.hpp"
#include <iostream>

namespace beast = boost::beast;
namespace http  = beast::http;

TetrisMasterServer::TetrisMasterServer(
    const std::string& httpAddress,
    const unsigned short httpPort,
    const std::string& udpAddress,
    const unsigned short udpPort,
    const std::string& dbHost,
    const std::string& dbPort,
    const std::string& gameHost,
    const std::string& gamePort
)
    : TetrisHTTPServer(httpAddress, httpPort)  // pass to TetrisHTTPServer constructor
    , TetrisUDPServer(udpAddress, udpPort)     // pass to TetrisUDPServer constructor
    , dbHost_(dbHost)
    , dbPort_(dbPort)
    , gameHost_(gameHost)
    , gamePort_(gamePort) {}

void TetrisMasterServer::run()
{
    // Start each server in its own thread
    httpThread_ = std::thread([this]() {
        TetrisHTTPServer::run();
    });

    udpThread_ = std::thread([this]() {
        TetrisUDPServer::run();
    });
}

void TetrisMasterServer::stop()
{
    // Stop both servers
    TetrisHTTPServer::stop();
    TetrisUDPServer::stop();

    // Join the threads if they are joinable
    if(httpThread_.joinable()) {
        httpThread_.join();
    }
    if(udpThread_.joinable()) {
        udpThread_.join();
    }
}

/**
 * 1) If request is for DB endpoints => forward to DB server
 * 2) If request is for game endpoints => forward to Game server
 * 3) Else => unknown
 */
void TetrisMasterServer::handleRequest(
    http::request<http::string_body>    req,
    http::response<http::string_body>&  res)
{
    namespace http = beast::http;

    const auto target = std::string(req.target());
    const http::verb verb = req.method();
    const std::string body = req.body();

    // DB Endpoints
    if (target == "/register"
     || target == "/login"
     || target == "/update"
     || target == "/add_friend"
     || target.rfind("/get_player", 0) == 0) // starts with /get_player
    {
        std::string dbResponse = forwardToDBServer(target, verb, body);
        res.result(http::status::ok);
        res.set(http::field::content_type, "application/json");
        res.body() = dbResponse.empty()
                     ? R"({"error":"No response from DB Server"})"
                     : dbResponse;
        res.prepare_payload();

        return;
    }

    // Game Endpoints
    if (target == "/create_lobby"
     || target == "/invite"
     || target == "/spectate"
     || target.rfind("/lobby_details", 0) == 0)
    {
        std::string gameResponse = forwardToGameServer(target, verb, body);
        res.result(http::status::ok);
        res.set(http::field::content_type, "application/json");
        res.body() = gameResponse.empty()
                     ? R"({"error":"No response from Game Server"})"
                     : gameResponse;
        res.prepare_payload();

        return;
    }

    // Unknown route
    res.result(http::status::bad_request);
    res.set(http::field::content_type, "application/json");
    res.body() = R"({"error":"Unknown endpoint"})";
    res.prepare_payload();
}

void TetrisMasterServer::handleUDPPacket(
    const std::string& data,
    const asio::ip::udp::endpoint& senderEndpoint)
{
    // TODO: parse the data as a keystroke (rather event) packet to be forwarded to the Game Server
    // For now, we log it
    std::cout << "[MasterServer:UDP] Received from "
              << senderEndpoint << ": " << data << std::endl;

    // Forward to game server by storing in a queue
}

// Helpers to forward to DB Server and Game Server
std::string TetrisMasterServer::forwardToDBServer(
    const std::string&  target,
    const http::verb    method,
    const std::string&  body)
{
    // Reuse TetrisHTTPServer::forwardRequest to talk to DB server
    return forwardRequest(dbHost_, dbPort_, target, method, body);
}

std::string TetrisMasterServer::forwardToGameServer(
    const std::string&  target,
    const http::verb    method,
    const std::string&  body)
{
    // Reuse TetrisHTTPServer::forwardRequest to talk to Game server
    return forwardRequest(gameHost_, gamePort_, target, method, body);
}