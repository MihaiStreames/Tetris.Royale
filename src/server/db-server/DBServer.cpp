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
std::unordered_map<std::string, Player>      TetrisDBServer::playerDB_{};
std::unordered_map<std::string, std::string> TetrisDBServer::userNameToAccountID_{};
std::mutex                                   TetrisDBServer::DBMutex_;

TetrisDBServer::TetrisDBServer(const std::string& address, const unsigned short port)
    : TetrisHTTPServer(address, port) {}

void TetrisDBServer::handleRequest(
    http::request<http::string_body>   req,
    http::response<http::string_body>& res)
{
    // Check the HTTP verb
    if (req.method() == http::verb::get) handleGetRequest(req, res);

    else if (req.method() == http::verb::post) {
        std::istringstream iss(req.body());
        boost::property_tree::ptree pt;

        try {
            read_json(iss, pt);
        } catch (...) {
            sendErrorResponse(res, http::status::bad_request, "Invalid JSON body", req.version());
            return;
        }

        handlePostRequest(std::string(req.target()), pt, req.version(), res);
    }

    else {
        sendErrorResponse(res, http::status::bad_request, "Unsupported HTTP method", req.version());
    }
}

void TetrisDBServer::handleGetRequest(
    const http::request<http::string_body>& req,
    http::response<http::string_body>&      res)
{
    // /get_leaderboard or /get_player?accountID=XYZ
    auto target = std::string(req.target());

    // /get_leaderboard
    if (target == "/get_leaderboard") {
        handleGetLeaderboard(req.version(), res);
        return;
    }

    // /get_player?accountID=xxxx
    {
        auto pos = target.find('?');
        if (pos != std::string::npos) {
            const std::string path     = target.substr(0, pos);
            const std::string queryStr = target.substr(pos + 1);

            if (path == "/get_player") {
                auto params = parseQuery(queryStr);
                auto it = params.find("accountID");
                if (it == params.end() || it->second.empty()) {
                    sendErrorResponse(res, http::status::bad_request, "Missing accountID param", req.version());
                    return;
                }
                handleGetPlayer(it->second, req.version(), res);
                return;
            }
        }
    }

    // If none of the above matched
    sendErrorResponse(res, http::status::not_found, "Unknown GET endpoint", req.version());
}

void TetrisDBServer::handleGetLeaderboard(
    const unsigned int version,
    http::response<http::string_body>& res)
{
    std::lock_guard lock(DBMutex_);

    // Copy players into a vector for sorting
    std::vector<Player> players;
    players.reserve(playerDB_.size());
    for (const auto& kv : playerDB_) {
        players.push_back(kv.second);
    }

    // Sort players by bestScore
    std::ranges::sort(players,
                      [](const Player& a, const Player& b) {
                          return a.bestScore > b.bestScore;
                      });

    // Build JSON
    boost::property_tree::ptree leaderboardNode;
    for (const auto& p : players) {
        boost::property_tree::ptree entry;
        entry.put("accountID", p.accountID);
        entry.put("userName",  p.userName);
        entry.put("bestScore", p.bestScore);
        leaderboardNode.push_back(std::make_pair("", entry));
    }

    boost::property_tree::ptree root;
    root.add_child("leaderboard", leaderboardNode);

    sendJSONResponse(res, http::status::ok, root, version);
}

void TetrisDBServer::handleGetPlayer(
    const std::string& accountID,
    const unsigned int version,
    http::response<http::string_body>& res)
{
    std::lock_guard lock(DBMutex_);
    const auto it = playerDB_.find(accountID);
    if (it == playerDB_.end()) {
        boost::property_tree::ptree pt;
        pt.put("error", "Account does not exist");
        sendJSONResponse(res, http::status::bad_request, pt, version);
        return;
    }

    const Player& p = it->second;

    boost::property_tree::ptree pt;
    pt.put("accountID", p.accountID);
    pt.put("userName",  p.userName);
    pt.put("bestScore", p.bestScore);

    // Convert friends to JSON array
    boost::property_tree::ptree friendsArray;
    vectorToJSONArray(p.friends, friendsArray);
    pt.add_child("friends", friendsArray);

    // Convert pending requests to JSON array
    boost::property_tree::ptree pendingArray;
    vectorToJSONArray(p.pendingFriendRequests, pendingArray);
    pt.add_child("pendingFriendRequests", pendingArray);

    sendJSONResponse(res, http::status::ok, pt, version);
}

void TetrisDBServer::handlePostRequest(
    const std::string&                 target,
    const boost::property_tree::ptree& pt,
    const unsigned int                 version,
    http::response<http::string_body>& res)
{
    // Routes (so far):
    //  /register
    //  /login
    //  /update
    //  /send_friend_request
    //  /add_friend
    //  /remove_friend

    if (target == "/register") {
        handleRegister(pt, version, res);
    }
    else if (target == "/login") {
        handleLogin(pt, version, res);
    }
    else if (target == "/update") {
        handleUpdate(pt, version, res);
    }
    else if (target == "/send_friend_request") {
        handleSendFriendRequest(pt, version, res);
    }
    else if (target == "/add_friend") {
        handleAddFriend(pt, version, res);
    }
    else if (target == "/remove_friend") {
        handleRemoveFriend(pt, version, res);
    }
    else {
        sendErrorResponse(res, http::status::not_found, "Unknown POST endpoint", version);
    }
}

// POST /register [Expected: userName, password]
void TetrisDBServer::handleRegister(
    const boost::property_tree::ptree& pt,
    const unsigned int version,
    http::response<http::string_body>& res)
{
    using namespace boost::beast::http;

    const auto userName = pt.get<std::string>("userName", "");
    const auto password = pt.get<std::string>("password", "");

    if (userName.empty() || password.empty()) {
        sendErrorResponse(res, status::bad_request, "Missing userName or password", version);
        return;
    }

    std::lock_guard lock(DBMutex_);

    if (userNameToAccountID_.contains(userName)) {
        boost::property_tree::ptree err;
        err.put("error", "User already exists");
        sendJSONResponse(res, status::bad_request, err, version);
        return;
    }

    // Create a new account
    const std::string newAccountID = generateUUID();

    Player p;
    p.accountID      = newAccountID;
    p.userName       = userName;
    p.hashedPassword = sha256Hash(password);
    p.bestScore      = 0;

    // Insert in DB
    playerDB_[newAccountID]        = p;
    userNameToAccountID_[userName] = newAccountID;

    // Return minimal info
    boost::property_tree::ptree resp;
    resp.put("accountID", newAccountID);
    resp.put("userName",  userName);
    sendJSONResponse(res, status::ok, resp, version);
}

// POST /login [Expected: userName, password]
void TetrisDBServer::handleLogin(
    const boost::property_tree::ptree& pt,
    const unsigned int version,
    http::response<http::string_body>& res)
{
    using namespace boost::beast::http;

    const auto userName = pt.get<std::string>("userName", "");
    const auto password = pt.get<std::string>("password", "");

    if (userName.empty() || password.empty()) {
        sendErrorResponse(res, status::bad_request, "Missing userName or password", version);
        return;
    }

    std::lock_guard lock(DBMutex_);

    const auto userIt = userNameToAccountID_.find(userName);
    if (userIt == userNameToAccountID_.end()) {
        boost::property_tree::ptree err;
        err.put("error", "User does not exist");
        sendJSONResponse(res, status::bad_request, err, version);
        return;
    }

    const std::string& accountID = userIt->second;
    const auto dbIt = playerDB_.find(accountID);
    if (dbIt == playerDB_.end()) {
        // This theoretically shouldn't happen if the map is in sync
        boost::property_tree::ptree err;
        err.put("error", "Account data not found");
        sendJSONResponse(res, status::internal_server_error, err, version);
        return;
    }

    const Player& p = dbIt->second;
    if (p.hashedPassword != sha256Hash(password)) {
        boost::property_tree::ptree err;
        err.put("error", "Invalid password");
        sendJSONResponse(res, status::unauthorized, err, version);
        return;
    }

    boost::property_tree::ptree resp;
    resp.put("message",   "Logged in successfully");
    resp.put("accountID", accountID);
    resp.put("userName",  userName);
    sendJSONResponse(res, status::ok, resp, version);
}

// POST /update [Expected: accountID, optional newUserName, optional newPassword]
void TetrisDBServer::handleUpdate(
    const boost::property_tree::ptree& pt,
    const unsigned int version,
    http::response<http::string_body>& res)
{
    using namespace boost::beast::http;

    const auto accountID = pt.get<std::string>("accountID", "");

    if (accountID.empty()) {
        sendErrorResponse(res, status::bad_request, "Missing accountID", version);
        return;
    }

    std::lock_guard lock(DBMutex_);
    const auto dbIt = playerDB_.find(accountID);
    if (dbIt == playerDB_.end()) {
        boost::property_tree::ptree err;
        err.put("error", "Account does not exist");
        sendJSONResponse(res, status::bad_request, err, version);
        return;
    }

    Player& p = dbIt->second;
    // Optional new fields
    const auto newUserName  = pt.get<std::string>("newName", "");
    const auto newPassword  = pt.get<std::string>("newPassword", "");

    // If changing userName, must also update userNameToAccountID_ map
    if (!newUserName.empty()) {
        if (userNameToAccountID_.contains(newUserName)) {
            boost::property_tree::ptree err;
            err.put("error", "Username already taken");
            sendJSONResponse(res, status::bad_request, err, version);
            return;
        }

        // Update data
        userNameToAccountID_.erase(p.userName);
        p.userName = newUserName;
        userNameToAccountID_[newUserName] = accountID;
    }

    if (!newPassword.empty()) {
        p.hashedPassword = sha256Hash(newPassword);
    }

    // Return updated data
    boost::property_tree::ptree resp;
    resp.put("accountID", p.accountID);
    resp.put("userName",  p.userName);
    sendJSONResponse(res, status::ok, resp, version);
}

// POST /send_friend_request [Expected: accountID (sender), otherAccountID (recipient)]
void TetrisDBServer::handleSendFriendRequest(
    const boost::property_tree::ptree& pt,
    const unsigned int version,
    http::response<http::string_body>& res)
{
    using namespace boost::beast::http;

    const auto sender   = pt.get<std::string>("accountID", "");
    const auto receiver = pt.get<std::string>("otherAccountID", "");
    if (sender.empty() || receiver.empty()) {
        sendErrorResponse(res, status::bad_request, "Missing accountID or otherAccountID", version);
        return;
    }

    std::lock_guard lock(DBMutex_);
    const auto itSender = playerDB_.find(sender);
    const auto itRecver = playerDB_.find(receiver);
    if (itSender == playerDB_.end() || itRecver == playerDB_.end()) {
        boost::property_tree::ptree err;
        err.put("error", "One or both accounts do not exist");
        sendJSONResponse(res, status::bad_request, err, version);
        return;
    }

    // Add sender to receiver's pending list
    Player& recverPlayer = itRecver->second;
    bool added = addUnique(recverPlayer.pendingFriendRequests, sender);
    if (!added) {
        boost::property_tree::ptree err;
        err.put("error", "Friend request already pending");
        sendJSONResponse(res, status::bad_request, err, version);
        return;
    }

    // Return success
    boost::property_tree::ptree resp;
    resp.put("message", "Friend request sent");
    resp.put("accountID", sender);
    resp.put("otherAccountID", receiver);
    sendJSONResponse(res, status::ok, resp, version);
}

// POST /add_friend [Expected: accountID, otherAccountID]
// "accountID" is the user ACCEPTING the friend request (the receiver)
// "otherAccountID" is the user who originally sent the request
void TetrisDBServer::handleAddFriend(
    const boost::property_tree::ptree& pt,
    const unsigned int version,
    http::response<http::string_body>& res)
{
    using namespace boost::beast::http;

    const auto acc = pt.get<std::string>("accountID", "");
    const auto oth = pt.get<std::string>("otherAccountID", "");

    if (acc.empty() || oth.empty()) {
        sendErrorResponse(res, status::bad_request, "Missing accountID or otherAccountID", version);
        return;
    }

    std::lock_guard lock(DBMutex_);
    const auto it1 = playerDB_.find(acc);
    const auto it2 = playerDB_.find(oth);
    if (it1 == playerDB_.end() || it2 == playerDB_.end()) {
        boost::property_tree::ptree err;
        err.put("error", "One or both accounts do not exist");
        sendJSONResponse(res, status::bad_request, err, version);
        return;
    }

    Player& p1 = it1->second;  // the receiver, who is calling /add_friend
    Player& p2 = it2->second;  // the sender, who created the friend request

    // Ensure that p1 has a pending request from p2
    if (!containsValue(p1.pendingFriendRequests, p2.accountID)) {
        boost::property_tree::ptree err;
        err.put("error", "No pending friend request from this user");
        sendJSONResponse(res, status::bad_request, err, version);
        return;
    }

    removeValue(p1.pendingFriendRequests, p2.accountID);

    // Add each other as friends
    const bool added1 = addUnique(p1.friends, p2.accountID);
    const bool added2 = addUnique(p2.friends, p1.accountID);

    if (!added1 && !added2) {
        boost::property_tree::ptree err;
        err.put("error", "Already friends");
        sendJSONResponse(res, status::bad_request, err, version);
        return;
    }

    // Return updated info about p1
    boost::property_tree::ptree resp;
    resp.put("accountID", p1.accountID);
    resp.put("userName",  p1.userName);

    // Convert updated friend list to JSON
    boost::property_tree::ptree friendsArray;
    vectorToJSONArray(p1.friends, friendsArray);
    resp.add_child("friends", friendsArray);

    sendJSONResponse(res, status::ok, resp, version);
}

// POST /remove_friend [Expected: accountID, otherAccountID]
void TetrisDBServer::handleRemoveFriend(
    const boost::property_tree::ptree& pt,
    const unsigned int version,
    http::response<http::string_body>& res)
{
    using namespace boost::beast::http;

    const auto acc = pt.get<std::string>("accountID", "");
    const auto oth = pt.get<std::string>("otherAccountID", "");

    if (acc.empty() || oth.empty()) {
        sendErrorResponse(res, status::bad_request, "Missing accountID or otherAccountID", version);
        return;
    }

    std::lock_guard lock(DBMutex_);
    const auto it1 = playerDB_.find(acc);
    const auto it2 = playerDB_.find(oth);
    if (it1 == playerDB_.end() || it2 == playerDB_.end()) {
        boost::property_tree::ptree err;
        err.put("error", "One or both accounts do not exist");
        sendJSONResponse(res, status::bad_request, err, version);
        return;
    }

    Player& p1 = it1->second;
    Player& p2 = it2->second;

    const bool removed1 = removeValue(p1.friends, oth);
    const bool removed2 = removeValue(p2.friends, acc);

    boost::property_tree::ptree resp;
    if (!removed1 && !removed2) {
        resp.put("message", "They weren't friends to begin with?");
    } else {
        resp.put("message", "Friend removed successfully");
    }

    sendJSONResponse(res, status::ok, resp, version);
}

// -------------------------------------- Utility --------------------------------------
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

std::string TetrisDBServer::buildJSON(const boost::property_tree::ptree& pt) {
    std::ostringstream oss;
    write_json(oss, pt, false);
    return oss.str();
}

void TetrisDBServer::vectorToJSONArray(
    const std::vector<std::string>& vec,
    boost::property_tree::ptree& arrayNode)
{
    for (const auto& val : vec) {
        boost::property_tree::ptree item;
        item.put("", val);
        arrayNode.push_back(std::make_pair("", item));
    }
}

void TetrisDBServer::sendJSONResponse(
    http::response<http::string_body>& res,
    const http::status                 status,
    const boost::property_tree::ptree& pt,
    const unsigned int                 version)
{
    res = http::response<http::string_body>(status, version);
    res.set(http::field::content_type, "application/json");
    res.body() = buildJSON(pt);
    res.prepare_payload();
}

void TetrisDBServer::sendErrorResponse(
    http::response<http::string_body>& res,
    const http::status                 status,
    const std::string&                 message,
    const unsigned int                 version)
{
    res = http::response<http::string_body>(status, version);
    res.set(http::field::content_type, "text/plain");
    res.body() = message;
    res.prepare_payload();
}

std::unordered_map<std::string, std::string> TetrisDBServer::parseQuery(
    const std::string &query)
{
    std::unordered_map<std::string, std::string> params;
    std::istringstream iss(query);
    std::string token;
    while (std::getline(iss, token, '&')) {
        if (const auto pos = token.find('='); pos != std::string::npos) {
            std::string key   = token.substr(0, pos);
            const std::string value = token.substr(pos+1);
            params[key] = value;
        }
    }
    return params;
}

bool TetrisDBServer::addUnique(std::vector<std::string>& v, const std::string& value)
{
    if (containsValue(v, value)) {
        return false;
    }
    v.push_back(value);
    return true;
}

bool TetrisDBServer::containsValue(const std::vector<std::string>& vec, const std::string& val) { return std::ranges::find(vec, val) != vec.end(); }

bool TetrisDBServer::removeValue(std::vector<std::string>& v, const std::string& value)
{
    const auto it = std::ranges::remove(v, value).begin();
    if (it == v.end()) {
        return false;
    }
    v.erase(it, v.end());
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