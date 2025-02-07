#include <boost/property_tree/json_parser.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <openssl/sha.h>     // For SHA256 hashing
#include <iostream>
#include <mutex>
#include <sstream>
#include <unordered_map>
#include <iomanip>

#include "dbServer.hpp"

namespace beast = boost::beast;
namespace http  = beast::http;
namespace asio  = boost::asio;
using tcp = asio::ip::tcp;

std::unordered_map<std::string, Player> playerDB;
std::mutex DBMutex;

std::string generateUUID() {
    boost::uuids::random_generator generator;
    const boost::uuids::uuid uuid = generator();
    return to_string(uuid);
}

std::string sha256Hash(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);
    std::stringstream ss;
    ss << std::hex << std::setfill('0');

    // Convert hash bytes to hex string
    for (const unsigned char i : hash)
        ss << std::setw(2) << static_cast<int>(i);

    return ss.str();
}

std::unordered_map<std::string, std::string> parseQuery(const std::string &query) {
    std::unordered_map<std::string, std::string> params;
    std::istringstream iss(query);
    std::string token;

    while (std::getline(iss, token, '&')) {
        if (const auto pos = token.find('='); pos != std::string::npos) {
            std::string key = token.substr(0, pos);
            const std::string value = token.substr(pos + 1);
            params[key] = value;
        }
    }

    return params;
}

std::string buildJSON(const boost::property_tree::ptree& pt) {
    std::ostringstream oss;
    write_json(oss, pt, false);
    return oss.str();
}

bool addFriendUUID(std::string &list, const std::string &uuid) {
    if (!list.empty()) {
        std::istringstream iss(list);
        std::string token;

        while (std::getline(iss, token, ',')) {
            if (token == uuid)
                return false;  // Already present
        }
    }

    if (list.empty())
        list = uuid;
    else
        list += "," + uuid;

    return true;
}

auto sendJSONResponse(
    http::response<http::string_body> &res,
    const http::status status,
    const boost::property_tree::ptree &pt,
    const unsigned int version) -> void
{
    res = http::response<http::string_body>(status, version);
    res.set(http::field::content_type, "application/json");
    res.body() = buildJSON(pt);
    res.prepare_payload();
}

auto sendErrorResponse(
    http::response<http::string_body> &res,
    const http::status status,
    const std::string &message,
    const unsigned int version) -> void
{
    res = http::response<http::string_body>(status, version);
    res.set(http::field::content_type, "text/plain");
    res.body() = message;
    res.prepare_payload();
}

void handleGetRequest(
    const http::request<http::string_body>& req,
    http::response<http::string_body>& res)
{
    // Extract the target and query string
    auto target = std::string(req.target());
    const auto pos = target.find('?');
    if (pos == std::string::npos) {
        sendErrorResponse(res, http::status::bad_request, "Missing query parameters", req.version());
        return;
    }

    const std::string path = target.substr(0, pos);
    const std::string queryStr = target.substr(pos + 1);
    if (path != "/get_player") {
        sendErrorResponse(res, http::status::not_found, "GET endpoint not found", req.version());
        return;
    }

    auto params = parseQuery(queryStr);
    const auto it = params.find("name");
    if (it == params.end() || it->second.empty()) {
        sendErrorResponse(res, http::status::bad_request, "Missing name parameter", req.version());
        return;
    }

    const std::string name = it->second;
    std::lock_guard lock(DBMutex);
    const auto playerIt = playerDB.find(name);
    if (playerIt == playerDB.end()) {
        boost::property_tree::ptree pt;
        pt.put("error", "Player does not exist");
        sendJSONResponse(res, http::status::bad_request, pt, req.version());
        return;
    }

    // All non-private player data is sent back
    const Player &p = playerIt->second;
    boost::property_tree::ptree pt;
    pt.put("uuid", p.uuid);
    pt.put("name", p.name);
    pt.put("bestScore", p.bestScore);
    pt.put("friendList", p.friendList);
    sendJSONResponse(res, http::status::ok, pt, req.version());
}

void handleRegister(
    const boost::property_tree::ptree& pt,
    const unsigned int version,
    http::response<http::string_body>& res)
{
    const auto name = pt.get<std::string>("name", "");
    const auto password = pt.get<std::string>("password", "");
    if (name.empty() || password.empty()) {
        sendErrorResponse(res, http::status::bad_request, "Missing name or password", version);
        return;
    }

    std::lock_guard lock(DBMutex);
    if (playerDB.contains(name)) {
        boost::property_tree::ptree err;
        err.put("error", "Player already exists");
        sendJSONResponse(res, http::status::bad_request, err, version);
        return;
    }

    // Create a new player
    Player p;
    p.uuid = generateUUID();
    p.name = name;
    p.hashedPassword = sha256Hash(password);
    p.friendList = "";
    p.bestScore = 0;
    playerDB[name] = p;

    // Send back just uuid and name as the get request above gives all the data needed for the client-side
    boost::property_tree::ptree resp;
    resp.put("uuid", p.uuid);
    resp.put("name", p.name);
    sendJSONResponse(res, http::status::ok, resp, version);
}

void handleLogin(
    const boost::property_tree::ptree& pt,
    const unsigned int version,
    http::response<http::string_body>& res)
{
    const auto name = pt.get<std::string>("name", "");
    const auto password = pt.get<std::string>("password", "");
    if (name.empty() || password.empty()) {
        sendErrorResponse(res, http::status::bad_request, "Missing name or password", version);
        return;
    }

    std::lock_guard lock(DBMutex);
    const auto it = playerDB.find(name);
    if (it == playerDB.end()) {
        boost::property_tree::ptree err;
        err.put("error", "Player does not exist");
        sendJSONResponse(res, http::status::bad_request, err, version);
        return;
    }

    const Player& p = it->second;
    if (p.hashedPassword != sha256Hash(password)) {
        boost::property_tree::ptree err;
        err.put("error", "Invalid password");
        sendJSONResponse(res, http::status::unauthorized, err, version);
        return;
    }

    // All that client-side has to do is show the message on screen
    boost::property_tree::ptree resp;
    resp.put("message", "Successfully logged in as " + name);
    sendJSONResponse(res, http::status::ok, resp, version);
}

void handleUpdate(
    const boost::property_tree::ptree& pt,
    unsigned int version,
    http::response<http::string_body>& res)
{
    auto name = pt.get<std::string>("name", "");
    auto password = pt.get<std::string>("password", "");
    if (name.empty() || password.empty()) {
        sendErrorResponse(res, http::status::bad_request, "Missing name or password", version);
        return;
    }

    std::lock_guard lock(DBMutex);
    auto it = playerDB.find(name);
    if (it == playerDB.end()) {
        boost::property_tree::ptree err;
        err.put("error", "Player does not exist");
        sendJSONResponse(res, http::status::bad_request, err, version);
        return;
    }

    Player& p = it->second;
    if (p.hashedPassword != sha256Hash(password)) {
        boost::property_tree::ptree err;
        err.put("error", "Invalid password");
        sendJSONResponse(res, http::status::unauthorized, err, version);
        return;
    }

    // Get new values if provided
    auto newName = pt.get<std::string>("newName", "");
    auto newPassword = pt.get<std::string>("newPassword", "");
    if (!newName.empty()) {
        if (playerDB.contains(newName)) {
            boost::property_tree::ptree err;
            err.put("error", "New username already taken");
            sendJSONResponse(res, http::status::bad_request, err, version);
            return;
        }
        // Update username: remove old entry and insert with new key
        Player updated = p;
        updated.name = newName;
        playerDB.erase(it);
        playerDB[newName] = updated;
        p = updated;
    }

    if (!newPassword.empty()) {
        p.hashedPassword = sha256Hash(newPassword);
    }

    // Probably not needed, does leverage some client-side work by sending back name
    boost::property_tree::ptree resp;
    resp.put("uuid", p.uuid);
    resp.put("name", p.name);
    sendJSONResponse(res, http::status::ok, resp, version);
}

void handleAddFriend(
    const boost::property_tree::ptree& pt,
    const unsigned int version,
    http::response<http::string_body>& res)
{
    const auto name = pt.get<std::string>("name", "");
    const std::string friendName = pt.get<std::string>("friendName", "");
    if (name.empty() || friendName.empty()) {
        sendErrorResponse(res, http::status::bad_request, "Missing required fields", version);
        return;
    }

    std::lock_guard lock(DBMutex);
    const auto it1 = playerDB.find(name);
    const auto it2 = playerDB.find(friendName);
    if (it1 == playerDB.end() || it2 == playerDB.end()) {
        boost::property_tree::ptree err;
        err.put("error", "One or both players do not exist");
        sendJSONResponse(res, http::status::bad_request, err, version);
        return;
    }

    Player& p = it1->second;
    Player& friendPlayer = it2->second;
    const bool added1 = addFriendUUID(p.friendList, friendPlayer.uuid);
    if (const bool added2 = addFriendUUID(friendPlayer.friendList, p.uuid); !added1 || !added2) {
        boost::property_tree::ptree err;
        err.put("error", "Friend already added");
        sendJSONResponse(res, http::status::bad_request, err, version);
        return;
    }

    // Send back relevant player details for client-side
    boost::property_tree::ptree resp;
    resp.put("uuid", p.uuid);
    resp.put("name", p.name);
    resp.put("friendList", p.friendList);
    sendJSONResponse(res, http::status::ok, resp, version);
}

void handleRequest(
    http::request<http::string_body> req,
    http::response<http::string_body>& res)
{
    if (req.method() == http::verb::get) {
        // Handle GET endpoints
        handleGetRequest(req, res);
    }

    else if (req.method() == http::verb::post) {
        // For POST requests, parse the JSON body
        std::istringstream iss(req.body());
        boost::property_tree::ptree pt;

        try {
            read_json(iss, pt);
        } catch (std::exception& e) { // More explicit error handling later
            sendErrorResponse(res, http::status::bad_request, "Invalid JSON", req.version());
            return;
        }

        if (const auto target = std::string(req.target()); target == "/register") {
            handleRegister(pt, req.version(), res);
        } else if (target == "/login") {
            handleLogin(pt, req.version(), res);
        } else if (target == "/update") {
            handleUpdate(pt, req.version(), res);
        } else if (target == "/add_friend") {
            handleAddFriend(pt, req.version(), res);
        } else {
            sendErrorResponse(res, http::status::not_found, "Endpoint not found", req.version());
        }
    }

    else {
        sendErrorResponse(res, http::status::bad_request, "Unsupported HTTP method", req.version());
    }
}

void doSession(tcp::socket socket) {
    constexpr bool close = false;
    beast::error_code ec;
    beast::flat_buffer buffer;

    while (!close) {
        http::request<http::string_body> req;
        http::read(socket, buffer, req, ec);

        if (ec == http::error::end_of_stream)
            break;
        if (ec) {
            std::cerr << "Read error: " << ec.message() << std::endl;
            break;
        }

        http::response<http::string_body> res;
        handleRequest(std::move(req), res);
        http::write(socket, res, ec);

        if (ec) {
            std::cerr << "Write error: " << ec.message() << std::endl;
            break;
        }

        if (res.need_eof())
            break;
    }

    socket.shutdown(tcp::socket::shutdown_send, ec);
}

int main(int argc, char* argv[]) {
    try {
        auto const address = asio::ip::make_address("0.0.0.0");
        unsigned short port = 8081; // DB Server port, do not forget
        asio::io_context ioc{1};
        tcp::acceptor acceptor{ioc, {address, port}};
        std::cout << "DB Server listening on port " << port << std::endl;

        for (;;) {
            tcp::socket socket{ioc};
            acceptor.accept(socket);
            // Launch each session in its own thread
            std::thread([sock = std::move(socket)]() mutable {
                doSession(std::move(sock));
            }).detach();
        }

    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}