#pragma once

#include <boost/property_tree/ptree.hpp>
#include <mutex>
#include <string>
#include <unordered_map>
#include "../http-server/HTTPServer.hpp"

struct Player {
    std::string uuid;
    std::string name;
    std::string hashedPassword;
    std::string friendList;
    int         bestScore;
};

class TetrisDBServer final : public TetrisHTTPServer {
public:
    TetrisDBServer(const std::string& address, unsigned short port);

protected:
    /**
     * Overrides TetrisHTTPServer's handleRequest to provide DB-specific logic
     */
    void handleRequest(
        http::request<http::string_body>    req,
        http::response<http::string_body>&  res
    ) override;

private:
    // Helper methods for each endpoint
    static void handleGetRequest(
        const http::request<http::string_body>& req,
        http::response<http::string_body>& res
    );

    static void handleRegister(
        const boost::property_tree::ptree& pt,
        unsigned int version,
        http::response<http::string_body>& res
    );

    static void handleLogin(
        const boost::property_tree::ptree& pt,
        unsigned int version,
        http::response<http::string_body>& res
    );

    static void handleUpdate(
        const boost::property_tree::ptree& pt,
        unsigned int version,
        http::response<http::string_body>& res
    );

    static void handleAddFriend(
        const boost::property_tree::ptree& pt,
        unsigned int version,
        http::response<http::string_body>& res
    );

    // Utility functions
    static std::string generateUUID();
    static std::string sha256Hash(const std::string& input);
    static std::string buildJSON(const boost::property_tree::ptree& pt);

    static void sendJSONResponse(
        http::response<http::string_body>& res,
        http::status status,
        const boost::property_tree::ptree& pt,
        unsigned int version
    );

    static void sendErrorResponse(
        http::response<http::string_body>& res,
        http::status status,
        const std::string& message,
        unsigned int version
    );

    static std::unordered_map<std::string, std::string> parseQuery(const std::string &query);
    static bool addFriendUUID(std::string &list, const std::string &uuid);

    // Database storage in memory for now
    static std::unordered_map<std::string, Player> playerDB_;
    static std::mutex DBMutex_;
};