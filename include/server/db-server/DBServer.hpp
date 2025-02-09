#pragma once

#include <boost/property_tree/ptree.hpp>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include "HTTPServer.hpp"

struct Player {
    std::string              accountID;
    std::string              userName;
    std::string              hashedPassword;
    std::vector<std::string> friends;
    std::vector<std::string> pendingFriendRequests;
    int                      bestScore;
};

class TetrisDBServer final : public TetrisHTTPServer {
public:
    TetrisDBServer(const std::string& address, unsigned short port);

protected:
    /**
     * Overrides TetrisHTTPServer's handleRequest to provide DB-specific logic
     */
    void handleRequest(
        http::request<http::string_body>   req,
        http::response<http::string_body>& res
    ) override;

private:
    // Helper methods for each endpoint
    static void handleGetRequest(
        const http::request<http::string_body>& req,
        http::response<http::string_body>& res
    );

    static void handlePostRequest(
        const std::string&                 target,
        const boost::property_tree::ptree& pt,
        unsigned int                       version,
        http::response<http::string_body>& res
    );

    // Endpoint handlers
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

    static void handleGetLeaderboard(
        unsigned int version,
        http::response<http::string_body>& res
    );

    static void handleGetPlayer(
        const std::string& accountID,
        unsigned int       version,
        http::response<http::string_body>& res
    );

    static void handleSendFriendRequest(
        const boost::property_tree::ptree& pt,
        unsigned int version,
        http::response<http::string_body>& res
    );

    static void handleAddFriend(
        const boost::property_tree::ptree& pt,
        unsigned int version,
        http::response<http::string_body>& res
    );

    static void handleRemoveFriend(
        const boost::property_tree::ptree& pt,
        unsigned int version,
        http::response<http::string_body>& res
    );

    // Utility functions
    static std::string generateUUID();
    static std::string sha256Hash(const std::string& input);
    static std::string buildJSON(const boost::property_tree::ptree& pt);
    static void        vectorToJSONArray(const std::vector<std::string>& vec, boost::property_tree::ptree& arrayNode);

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

    // Vector friend helpers
    static bool addUnique(std::vector<std::string>& v, const std::string& value);
    static bool removeValue(std::vector<std::string>& v, const std::string& value);
    static bool containsValue(const std::vector<std::string>& vec, const std::string& val);

    // Database storage in memory
    //  - playerDB_: maps accountID -> Player
    //  - userNameToAccountID_: maps userName -> accountID
    static std::unordered_map<std::string, Player>      playerDB_;
    static std::unordered_map<std::string, std::string> userNameToAccountID_;
    static std::mutex                                   DBMutex_;
};