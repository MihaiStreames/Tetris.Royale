#pragma once

#include "HTTPServer.hpp"
#include "UDPServer.hpp"
#include <string>
#include <boost/beast/http.hpp>

class TetrisMasterServer final : public TetrisHTTPServer, public TetrisUDPServer
{
public:
    /**
     * Constructor – you provide:
     *   @param httpAddress   e.g. "0.0.0.0"
     *   @param httpPort      e.g. 8080 (for incoming HTTP)
     *   @param udpAddress    e.g. "0.0.0.0"
     *   @param udpPort       e.g. 9000 (for incoming UDP)
     *   @param dbHost        e.g. "127.0.0.1"
     *   @param dbPort        e.g. "8081"
     *   @param gameHost      e.g. "127.0.0.1"
     *   @param gamePort      e.g. "9001"
     */
    TetrisMasterServer(
        const std::string& httpAddress,
        unsigned short httpPort,
        const std::string& udpAddress,
        unsigned short udpPort,
        const std::string& dbHost,
        const std::string& dbPort,
        const std::string& gameHost,
        const std::string& gamePort
    );

    void run() override;
    void stop() override;

protected:
    /**
     * Override TetrisHTTPServer method to handle or forward requests.
     */
    void handleRequest(
        http::request<http::string_body>    req,
        http::response<http::string_body>&  res
    ) override;

    /**
     * Override TetrisUDPServer method to handle incoming UDP data
     * (keystrokes -> mostly events)
     */
    void handleUDPPacket(
        const std::string& data,
        const asio::ip::udp::endpoint& senderEndpoint
    ) override;

private:
    // Helper to forward requests to DB or Game server
    std::string forwardToDBServer(
        const std::string& target,
        http::verb method,
        const std::string& body
    );

    std::string forwardToGameServer(
        const std::string& target,
        http::verb method,
        const std::string& body
    );

    std::string dbHost_;
    std::string dbPort_;
    std::string gameHost_;
    std::string gamePort_;

    std::thread httpThread_;
    std::thread udpThread_;
};