#ifndef DBSERVER_SQLITE_HPP
#define DBSERVER_SQLITE_HPP

#include <mutex>
#include <string>
#include <unordered_map>
#include <boost/property_tree/ptree.hpp>
#include <sqlite3.h>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <boost/property_tree/json_parser.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <openssl/sha.h>
#include <filesystem>
#include "HTTPServer.hpp"
#include "common.hpp"

class TetrisDBServer final : public TetrisHTTPServer {
public:
    // The constructor opens the SQLite DB file
    TetrisDBServer(const std::string &address, unsigned short port, const std::string &dbFile);

    ~TetrisDBServer() override;

    [[nodiscard]] StatusCode startDBServer();
    [[nodiscard]] StatusCode closeDBServer();
    [[nodiscard]] bool isDBServerRunning() const;

protected:
    // Overrides the HTTP server request handler
    void handleRequest(
        http::request<http::string_body> req,
        http::response<http::string_body> &res
    ) override;

private:
    sqlite3 *db_;
    std::mutex dbMutex_;
    std::thread dbThread_;
    std::atomic<bool> stopFlag_;
    std::atomic<bool> running_;

    // Important
    void dbServerLoop();  // Runs the HTTP server in a non-blocking thread
    void initializeDatabase() const;

    // Utility functions
    static void sendJSONResponse(
        http::response<http::string_body> &res,
        http::status status,
        const boost::property_tree::ptree &pt,
        unsigned int version
    );

    static void sendErrorResponse(
        http::response<http::string_body> &res,
        http::status status,
        const std::string &message,
        unsigned int version
    );

    static std::unordered_map<std::string, std::string> parseQuery(const std::string &query);

    // Request dispatch functions
    void handleGetRequest(
        const http::request<http::string_body> &req,
        http::response<http::string_body> &res
    );

    void handlePostRequest(
        const std::string &target,
        const boost::property_tree::ptree &pt,
        unsigned int version,
        http::response<http::string_body> &res
    );

    // Endpoint handlers
    void handleRegister(
        const boost::property_tree::ptree &pt,
        unsigned int version,
        http::response<http::string_body> &res
    );

    void handleLogin(
        const boost::property_tree::ptree &pt,
        unsigned int version,
        http::response<http::string_body> &res
    );

    void handleUpdate(
        const boost::property_tree::ptree &pt,
        unsigned int version,
        http::response<http::string_body> &res
    );

    void handlePostScore(
        const boost::property_tree::ptree &pt,
        unsigned int version,
        http::response<http::string_body> &res
    );

    void handleGetLeaderboard(
        unsigned int version,
        http::response<http::string_body> &res,
        const std::string &query
    );

    void handleGetPlayer(
        const std::string &accountID,
        unsigned int version,
        http::response<http::string_body> &res
    );

    void handleSendFriendRequest(
        const boost::property_tree::ptree &pt,
        unsigned int version,
        http::response<http::string_body> &res
    );

    void handleAcceptFriendRequest(
        const boost::property_tree::ptree &pt,
        unsigned int version,
        http::response<http::string_body> &res
    );

    void handleDeclineFriendRequest(
        const boost::property_tree::ptree &pt,
        unsigned int version,
        http::response<http::string_body> &res
    );

    void handleRemoveFriend(
        const boost::property_tree::ptree &pt,
        unsigned int version,
        http::response<http::string_body> &res
    );

    void handleGetMessages(
        const std::string &query,
        unsigned int version,
        http::response<http::string_body> &res
    );

    void handlePostMessage(
        const boost::property_tree::ptree &pt,
        unsigned int version,
        http::response<http::string_body> &res
    );

    void handleDeleteMessage(
        const boost::property_tree::ptree &pt,
        unsigned int version,
        http::response<http::string_body> &res
    );
};

#endif // DBSERVER_SQLITE_HPP
