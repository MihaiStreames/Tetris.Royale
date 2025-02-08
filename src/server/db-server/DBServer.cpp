#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <openssl/sha.h>
#include <sstream>
#include <iostream>
#include "DBServer.hpp"

namespace beast = boost::beast;
namespace http  = beast::http;

// Static class members
std::unordered_map<std::string, Player> TetrisDBServer::playerDB_{};
std::mutex TetrisDBServer::DBMutex_;

TetrisDBServer::TetrisDBServer(const std::string& address, const unsigned short port)
    : TetrisHTTPServer(address, port) {}

void TetrisDBServer::handleRequest(
    http::request<http::string_body>    req,
    http::response<http::string_body>&  res)
{
    // Check the HTTP verb
    if (req.method() == http::verb::get) {
        handleGetRequest(req, res);
    }

    else if (req.method() == http::verb::post) {
        // Parse JSON body for POST
        std::istringstream iss(req.body());
        boost::property_tree::ptree pt;

        try {
            read_json(iss, pt);
        }

        catch (std::exception&) {
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

// GET /get_player?name=<...>
void TetrisDBServer::handleGetRequest(
    const http::request<http::string_body>& req,
    http::response<http::string_body>& res)
{
    auto target = std::string(req.target());
    const auto pos = target.find('?');
    if (pos == std::string::npos) {
        sendErrorResponse(res, http::status::bad_request, "Missing query parameters", req.version());
        return;
    }

    const std::string path     = target.substr(0, pos);
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
    std::lock_guard lock(DBMutex_);
    const auto playerIt = playerDB_.find(name);
    if (playerIt == playerDB_.end()) {
        boost::property_tree::ptree pt;
        pt.put("error", "Player does not exist");
        sendJSONResponse(res, http::status::bad_request, pt, req.version());
        return;
    }

    // Return non-private data
    const Player &p = playerIt->second;
    boost::property_tree::ptree pt;
    pt.put("uuid",       p.uuid);
    pt.put("name",       p.name);
    pt.put("bestScore",  p.bestScore);
    pt.put("friendList", p.friendList);

    sendJSONResponse(res, http::status::ok, pt, req.version());
}

// POST /register
void TetrisDBServer::handleRegister(
    const boost::property_tree::ptree& pt,
    const unsigned int version,
    http::response<http::string_body>& res)
{
    using namespace boost::beast::http;

    const auto name     = pt.get<std::string>("name", "");
    const auto password = pt.get<std::string>("password", "");
    if (name.empty() || password.empty()) {
        sendErrorResponse(res, status::bad_request, "Missing name or password", version);
        return;
    }

    std::lock_guard lock(DBMutex_);
    if (playerDB_.contains(name)) {
        boost::property_tree::ptree err;
        err.put("error", "Player already exists");
        sendJSONResponse(res, status::bad_request, err, version);
        return;
    }

    // Create a new player
    Player p;
    p.uuid          = generateUUID();
    p.name          = name;
    p.hashedPassword= sha256Hash(password);
    p.friendList    = "";
    p.bestScore     = 0;
    playerDB_[name] = p;

    // Return relevant player info
    boost::property_tree::ptree resp;
    resp.put("uuid", p.uuid);
    resp.put("name", p.name); // Maybe not needed?
    sendJSONResponse(res, status::ok, resp, version);
}

// POST /login
void TetrisDBServer::handleLogin(
    const boost::property_tree::ptree& pt,
    const unsigned int version,
    http::response<http::string_body>& res)
{
    using namespace boost::beast::http;

    const auto name     = pt.get<std::string>("name", "");
    const auto password = pt.get<std::string>("password", "");
    if (name.empty() || password.empty()) {
        sendErrorResponse(res, status::bad_request, "Missing name or password", version);
        return;
    }

    std::lock_guard lock(DBMutex_);
    const auto it = playerDB_.find(name);
    if (it == playerDB_.end()) {
        boost::property_tree::ptree err;
        err.put("error", "Player does not exist");
        sendJSONResponse(res, status::bad_request, err, version);
        return;
    }

    const Player& p = it->second;
    if (p.hashedPassword != sha256Hash(password)) {
        boost::property_tree::ptree err;
        err.put("error", "Invalid password");
        sendJSONResponse(res, status::unauthorized, err, version);
        return;
    }

    boost::property_tree::ptree resp;
    resp.put("message", "Successfully logged in as " + name);
    sendJSONResponse(res, status::ok, resp, version);
}

// POST /update
void TetrisDBServer::handleUpdate(
    const boost::property_tree::ptree& pt,
    unsigned int version,
    http::response<http::string_body>& res)
{
    using namespace boost::beast::http;

    auto name     = pt.get<std::string>("name", "");
    auto password = pt.get<std::string>("password", "");
    if (name.empty() || password.empty()) {
        sendErrorResponse(res, status::bad_request, "Missing name or password", version);
        return;
    }

    std::lock_guard lock(DBMutex_);
    auto it = playerDB_.find(name);
    if (it == playerDB_.end()) {
        boost::property_tree::ptree err;
        err.put("error", "Player does not exist");
        sendJSONResponse(res, status::bad_request, err, version);
        return;
    }

    Player& p = it->second;
    if (p.hashedPassword != sha256Hash(password)) {
        boost::property_tree::ptree err;
        err.put("error", "Invalid password");
        sendJSONResponse(res, status::unauthorized, err, version);
        return;
    }

    // Get new values
    auto newName     = pt.get<std::string>("newName", "");
    auto newPassword = pt.get<std::string>("newPassword", "");
    if (!newName.empty()) {
        // Check if new username is taken
        if (playerDB_.contains(newName)) {
            boost::property_tree::ptree err;
            err.put("error", "New username already taken");
            sendJSONResponse(res, status::bad_request, err, version);
            return;
        }
        // Update key in the map
        Player updated = p;
        updated.name = newName;
        playerDB_.erase(it); // remove old entry
        playerDB_[newName] = updated;
        p = playerDB_[newName];
    }

    if (!newPassword.empty()) {
        p.hashedPassword = sha256Hash(newPassword);
    }

    // Return updated data
    boost::property_tree::ptree resp;
    resp.put("uuid", p.uuid);
    resp.put("name", p.name);
    sendJSONResponse(res, status::ok, resp, version);
}

// POST /add_friend
void TetrisDBServer::handleAddFriend(
    const boost::property_tree::ptree& pt,
    const unsigned int version,
    http::response<http::string_body>& res)
{
    using namespace boost::beast::http;

    const auto name       = pt.get<std::string>("name", "");
    const auto friendName = pt.get<std::string>("friendName", "");
    if (name.empty() || friendName.empty()) {
        sendErrorResponse(res, status::bad_request, "Missing required fields", version);
        return;
    }

    std::lock_guard lock(DBMutex_);
    const auto it1 = playerDB_.find(name);
    const auto it2 = playerDB_.find(friendName);
    if (it1 == playerDB_.end() || it2 == playerDB_.end()) {
        boost::property_tree::ptree err;
        err.put("error", "One or both players do not exist");
        sendJSONResponse(res, status::bad_request, err, version);
        return;
    }

    Player& p = it1->second;
    Player& friendPlayer = it2->second;
    const bool added1 = addFriendUUID(p.friendList, friendPlayer.uuid);
    const bool added2 = addFriendUUID(friendPlayer.friendList, p.uuid);

    if (!added1 || !added2) {
        boost::property_tree::ptree err;
        err.put("error", "Friend already added");
        sendJSONResponse(res, status::bad_request, err, version);
        return;
    }

    // Return updated info
    boost::property_tree::ptree resp;
    resp.put("uuid",       p.uuid);
    resp.put("name",       p.name);
    resp.put("friendList", p.friendList);
    sendJSONResponse(res, status::ok, resp, version);
}

std::string TetrisDBServer::generateUUID() {
    boost::uuids::random_generator generator;
    const boost::uuids::uuid uuid = generator();
    return to_string(uuid);
}

std::string TetrisDBServer::sha256Hash(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);

    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (const unsigned char i : hash) {
        ss << std::setw(2) << static_cast<int>(i);
    }

    return ss.str();
}

std::unordered_map<std::string, std::string> TetrisDBServer::parseQuery(const std::string &query) {
    std::unordered_map<std::string, std::string> params;
    std::istringstream iss(query);
    std::string token;

    while(std::getline(iss, token, '&')) {
        if(const auto pos = token.find('='); pos != std::string::npos) {
            std::string key   = token.substr(0, pos);
            const std::string value = token.substr(pos + 1);
            params[key] = value;
        }
    }

    return params;
}

std::string TetrisDBServer::buildJSON(const boost::property_tree::ptree& pt) {
    std::ostringstream oss;
    write_json(oss, pt, false);
    return oss.str();
}

void TetrisDBServer::sendJSONResponse(
    http::response<http::string_body>& res,
    const http::status status,
    const boost::property_tree::ptree& pt,
    const unsigned int version)
{
    using namespace boost::beast::http;

    res = response<string_body>(status, version);
    res.set(field::content_type, "application/json");
    res.body() = buildJSON(pt);
    res.prepare_payload();
}

void TetrisDBServer::sendErrorResponse(
    http::response<http::string_body>& res,
    const http::status status,
    const std::string& message,
    const unsigned int version)
{
    using namespace boost::beast::http;

    res = response<string_body>(status, version);
    res.set(field::content_type, "text/plain");
    res.body() = message;
    res.prepare_payload();
}

bool TetrisDBServer::addFriendUUID(std::string &list, const std::string &uuid) {
    if (!list.empty()) {
        std::istringstream iss(list);
        std::string token;

        while (std::getline(iss, token, ',')) {
            if (token == uuid) {
                return false; // Already present
            }
        }
    }

    if (list.empty()) {
        list = uuid;
    } else {
        list += "," + uuid;
    }

    return true;
}

int main() {
    try {
        TetrisDBServer dbServer("0.0.0.0", 8081);
        dbServer.run();
    }
    catch(const std::exception& e) {
        std::cerr << "DBServer error: " << e.what() << std::endl;
    }
    return 0; // success
}