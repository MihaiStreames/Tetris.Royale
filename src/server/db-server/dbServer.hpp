#ifndef DB_SERVER_HPP
#define DB_SERVER_HPP

#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>

namespace beast = boost::beast;
namespace http  = beast::http;
namespace asio  = boost::asio;
using tcp = asio::ip::tcp;

/**
 * @file dbServer.hpp
 * @brief API Documentation for the DB Server.
 *
 * This header file defines the API endpoints available in the database server.
 * Client-side developers should refer to this file for request/response formats.
 */

/**
 * @struct Player
 * @brief Represents a player in the database.
 */
struct Player {
    std::string uuid;           ///< Unique player ID (UUID format)
    std::string name;           ///< Player's username
    std::string hashedPassword; ///< SHA256-hashed password
    std::string friendList;     ///< Comma-separated list of friend UUIDs
    int bestScore = 0;          ///< Best score achieved in infinite mode
};

/**
 * @brief Handles the GET /get_player request.
 *
 * @param req HTTP request object.
 * @param res HTTP response object.
 *
 * @details
 * Endpoint: GET /get_player?name=<username>
 */
void handleGetRequest(const http::request<http::string_body>& req, http::response<http::string_body>& res);

/**
 * @brief Handles the POST /register request.
 *
 * @param pt Parsed JSON request body.
 * @param version HTTP version.
 * @param res HTTP response object.
 *
 * @details
 * Endpoint: POST /register
 */
void handleRegister(const boost::property_tree::ptree& pt, unsigned int version, http::response<http::string_body>& res);

/**
 * @brief Handles the POST /login request.
 *
 * @param pt Parsed JSON request body.
 * @param version HTTP version.
 * @param res HTTP response object.
 *
 * @details
 * Endpoint: POST /login
 */
void handleLogin(const boost::property_tree::ptree& pt, unsigned int version, http::response<http::string_body>& res);

/**
 * @brief Handles the POST /update request.
 *
 * @param pt Parsed JSON request body.
 * @param version HTTP version.
 * @param res HTTP response object.
 *
 * @details
 * Endpoint: POST /update
 */
void handleUpdate(const boost::property_tree::ptree& pt, unsigned int version, http::response<http::string_body>& res);

/**
 * @brief Handles the POST /add_friend request.
 *
 * @param pt Parsed JSON request body.
 * @param version HTTP version.
 * @param res HTTP response object.
 *
 * @details
 * Endpoint: POST /add_friend
 */
void handleAddFriend(const boost::property_tree::ptree& pt, unsigned int version, http::response<http::string_body>& res);

/**
 * @brief Dispatches HTTP requests to the correct endpoint handler.
 * @param req The incoming HTTP request.
 * @param res The HTTP response object.
 */
void handleRequest(http::request<http::string_body> req, http::response<http::string_body>& res);

/**
 * @brief Handles a client session (reads request, processes it, sends response).
 * @param socket The client's TCP socket.
 */
void doSession(tcp::socket socket);

#endif // DB_SERVER_HPP