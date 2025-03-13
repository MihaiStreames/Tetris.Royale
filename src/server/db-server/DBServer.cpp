#include "DBServer.hpp"

#include "DBCommon.hpp"

namespace fs = std::filesystem;
namespace beast = boost::beast;
namespace http = beast::http;

// ----------------------- Constructor / Destructor -----------------------
TetrisDBServer::TetrisDBServer(const std::string& address,
                               const unsigned short port,
                               const std::string& dbFile)
    : TetrisHTTPServer(address, port), db_(nullptr)
{
    try
    {
        const fs::path dbPath(dbFile);
        const fs::path dbFolder = dbPath.parent_path();

        // Ensure the database directory exists
        if (!exists(dbFolder))
        {
            std::cout << "[DBServer] Creating database directory: " << dbFolder
                      << std::endl;
            create_directories(dbFolder);
        }

        // Attempt to open the database
        if (sqlite3_open(dbFile.c_str(), &db_) != SQLITE_OK)
        {
            std::cerr << "Cannot open database: " << sqlite3_errmsg(db_)
                      << std::endl;
            std::exit(EXIT_FAILURE);
        }

        std::cout << "[DBServer] Database opened at " << dbFile << std::endl;
        initializeDatabase();
    }
    catch (const std::exception& e)
    {
        std::cerr << "[DBServer] Error initializing database: " << e.what()
                  << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

TetrisDBServer::~TetrisDBServer() { (void)closeDBServer(); }

StatusCode
TetrisDBServer::startDBServer()
{
    if (running_)
    {
        std::cerr << "[DBServer] [INFO] Already running.\n";
        return StatusCode::SUCCESS;
    }

    try
    {
        std::cout << "[DBServer] [INFO] Starting database server\n";
        stopFlag_ = false;

        // Start the HTTP server in a separate thread
        dbThread_ = std::thread(&TetrisDBServer::dbServerLoop, this);

        running_ = true;
        return StatusCode::SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << "[DBServer] [ERROR] Failed to start: " << e.what()
                  << std::endl;
        return StatusCode::ERROR;
    }
}

void
TetrisDBServer::dbServerLoop()
{
    std::cout << "[DBServer] [INFO] Database HTTP server running" << std::endl;

    // Start accepting HTTP requests
    this->run(); // TetrisHTTPServer::run()

    while (!stopFlag_)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "[DBServer] [INFO] Database HTTP server shutting down...\n";
}

StatusCode
TetrisDBServer::closeDBServer()
{
    if (!running_)
    {
        std::cerr << "[DBServer] [INFO] Not running\n";
        return StatusCode::SUCCESS;
    }

    try
    {
        std::cout << "[DBServer] [INFO] Stopping database server...\n";
        stopFlag_ = true; // Signal shutdown
        this->stop();     // Stop HTTP server

        if (dbThread_.joinable())
        {
            dbThread_.join(); // Wait for the server thread to finish
        }

        sqlite3_close(db_);
        db_ = nullptr;
        running_ = false;
        std::cout << "[DBServer] [INFO] Database server stopped\n";

        return StatusCode::SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << "[DBServer] [ERROR] Failed to close: " << e.what()
                  << std::endl;
        return StatusCode::ERROR;
    }
}

bool
TetrisDBServer::isDBServerRunning() const
{
    return running_;
}

// ----------------------- Database Initialization -----------------------
void
TetrisDBServer::initializeDatabase() const
{
    // Create tables if they do not exist.
    // Note: The players table no longer stores friend lists as JSON.
    const auto sql = R"sql(
        CREATE TABLE IF NOT EXISTS players (
            accountID TEXT PRIMARY KEY,
            sessionID TEXT UNIQUE,
            userName TEXT UNIQUE NOT NULL,
            hashedPassword TEXT NOT NULL,
            bestScore INTEGER DEFAULT 0
        );
        CREATE TABLE IF NOT EXISTS friends (
            accountID TEXT NOT NULL,
            friendID TEXT NOT NULL,
            PRIMARY KEY (accountID, friendID),
            FOREIGN KEY (accountID) REFERENCES players(accountID),
            FOREIGN KEY (friendID) REFERENCES players(accountID)
        );
        CREATE TABLE IF NOT EXISTS friend_requests (
            senderID TEXT NOT NULL,
            receiverID TEXT NOT NULL,
            PRIMARY KEY (senderID, receiverID),
            FOREIGN KEY (senderID) REFERENCES players(accountID),
            FOREIGN KEY (receiverID) REFERENCES players(accountID)
        );
        CREATE TABLE IF NOT EXISTS messages (
            messageID TEXT PRIMARY KEY,
            senderID TEXT NOT NULL,
            receiverID TEXT NOT NULL,
            content TEXT NOT NULL,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (senderID) REFERENCES players(accountID),
            FOREIGN KEY (receiverID) REFERENCES players(accountID)
        );
    )sql";

    char* errMsg = nullptr;
    if (sqlite3_exec(db_, sql, nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        std::cerr << "SQL error during initialization: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        std::exit(EXIT_FAILURE);
    }
}

// ----------------------- Utility Functions -----------------------
void
TetrisDBServer::sendJSONResponse(http::response<http::string_body>& res,
                                 const http::status status,
                                 const boost::property_tree::ptree& pt,
                                 const unsigned int version)
{
    res = http::response<http::string_body>(status, version);
    res.set(http::field::content_type, "application/json");
    res.body() = buildJSON(pt);
    res.prepare_payload();
}

void
TetrisDBServer::sendErrorResponse(http::response<http::string_body>& res,
                                  const http::status status,
                                  const std::string& message,
                                  const unsigned int version)
{
    res = http::response<http::string_body>(status, version);
    res.set(http::field::content_type, "text/plain");
    res.body() = message;
    res.prepare_payload();
}

std::unordered_map<std::string, std::string>
TetrisDBServer::parseQuery(const std::string& query)
{
    std::unordered_map<std::string, std::string> params;
    std::istringstream iss(query);
    std::string token;
    while (std::getline(iss, token, '&'))
    {
        if (const size_t pos = token.find('='); pos != std::string::npos)
        {
            std::string key = token.substr(0, pos);
            const std::string value = token.substr(pos + 1);
            params[key] = value;
        }
    }

    return params;
}

// ----------------------- HTTP Request Dispatch -----------------------
void
TetrisDBServer::handleRequest(http::request<http::string_body> req,
                              http::response<http::string_body>& res)
{
    if (req.method() == http::verb::get)
    {
        handleGetRequest(req, res);
    }

    else if (req.method() == http::verb::post)
    {
        std::istringstream iss(req.body());
        boost::property_tree::ptree pt;

        try
        {
            read_json(iss, pt);
        }
        catch (...)
        {
            sendErrorResponse(res, http::status::bad_request,
                              "Invalid JSON body", req.version());
            return;
        }

        handlePostRequest(std::string(req.target()), pt, req.version(), res);
    }
    else
    {
        sendErrorResponse(res, http::status::bad_request,
                          "Unsupported HTTP method", req.version());
    }
}

void
TetrisDBServer::handleGetRequest(const http::request<http::string_body>& req,
                                 http::response<http::string_body>& res)
{
    const std::string target(req.target());
    if (target.find("/get_leaderboard") == 0)
    {
        handleGetLeaderboard(req.version(), res, target);
    }
    else if (target.find("/get_player") == 0)
    {
        if (const auto pos = target.find('?'); pos != std::string::npos)
        {
            auto params = parseQuery(target.substr(pos + 1));

            if (!params.contains("accountID") || params["accountID"].empty())
            {
                sendErrorResponse(res, http::status::bad_request,
                                  "Missing accountID param", req.version());
                return;
            }

            handleGetPlayer(params["accountID"], req.version(), res);
            return;
        }

        sendErrorResponse(res, http::status::bad_request,
                          "Missing query parameters", req.version());
    }
    else if (target.find("/get_messages") == 0)
    {
        handleGetMessages(target, req.version(), res);
    }
    else
    {
        sendErrorResponse(res, http::status::not_found, "Unknown GET endpoint",
                          req.version());
    }
}

void
TetrisDBServer::handlePostRequest(const std::string& target,
                                  const boost::property_tree::ptree& pt,
                                  const unsigned int version,
                                  http::response<http::string_body>& res)
{
    if (target == "/register")
    {
        handleRegister(pt, version, res);
    }
    else if (target == "/login")
    {
        handleLogin(pt, version, res);
    }
    else if (target == "/update")
    {
        handleUpdate(pt, version, res);
    }
    else if (target == "/post_score")
    {
        handlePostScore(pt, version, res);
    }
    else if (target == "/send_friend_request")
    {
        handleSendFriendRequest(pt, version, res);
    }
    else if (target == "/accept_friend_request")
    {
        handleAcceptFriendRequest(pt, version, res);
    }
    else if (target == "/decline_friend_request")
    {
        handleDeclineFriendRequest(pt, version, res);
    }
    else if (target == "/remove_friend")
    {
        handleRemoveFriend(pt, version, res);
    }
    else if (target == "/post_message")
    {
        handlePostMessage(pt, version, res);
    }
    else if (target == "/delete_message")
    {
        handleDeleteMessage(pt, version, res);
    }
    else
    {
        sendErrorResponse(res, http::status::not_found, "Unknown POST endpoint",
                          version);
    }
}

// ----------------------- Endpoint Handlers -----------------------

// POST /register { "userName": "...", "password": "..." }
void
TetrisDBServer::handleRegister(const boost::property_tree::ptree& pt,
                               const unsigned int version,
                               http::response<http::string_body>& res)
{
    const std::string userName = pt.get<std::string>("userName", "");
    const std::string password = pt.get<std::string>("password", "");

    if (userName.empty() || password.empty())
    {
        sendErrorResponse(res, http::status::bad_request,
                          "Missing userName or password", version);
        return;
    }

    std::lock_guard lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;

    const auto checkSql = "SELECT userName FROM players WHERE userName = ?;";
    if (sqlite3_prepare_v2(db_, checkSql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sendErrorResponse(res, http::status::internal_server_error, "DB error",
                          version);
        return;
    }
    sqlite3_bind_text(stmt, 1, userName.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        sqlite3_finalize(stmt);
        sendErrorResponse(res, http::status::bad_request,
                          "User already exists!", version);
        return;
    }
    sqlite3_finalize(stmt);

    const std::string accountID = generateUUID();
    const std::string hashedPassword = sha256Hash(password);
    const auto insertSql =
        "INSERT INTO players (accountID, userName, hashedPassword) VALUES (?, "
        "?, ?);";
    if (sqlite3_prepare_v2(db_, insertSql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sendErrorResponse(res, http::status::internal_server_error, "DB error",
                          version);
        return;
    }
    sqlite3_bind_text(stmt, 1, accountID.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, userName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, hashedPassword.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        sendErrorResponse(res, http::status::internal_server_error,
                          "Failed to register user", version);
        return;
    }
    sqlite3_finalize(stmt);

    boost::property_tree::ptree resp;
    resp.put("accountID", accountID);
    resp.put("userName", userName);
    sendJSONResponse(res, http::status::ok, resp, version);
}

// POST /login { "userName": "...", "password": "..." }
void
TetrisDBServer::handleLogin(const boost::property_tree::ptree& pt,
                            const unsigned int version,
                            http::response<http::string_body>& res)
{
    const std::string userName = pt.get<std::string>("userName", "");
    const std::string password = pt.get<std::string>("password", "");
    if (userName.empty() || password.empty())
    {
        sendErrorResponse(res, http::status::bad_request,
                          "Missing userName or password", version);
        return;
    }

    std::lock_guard lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    const auto sql =
        "SELECT accountID, hashedPassword FROM players WHERE userName = ?;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sendErrorResponse(res, http::status::internal_server_error, "DB error",
                          version);
        return;
    }

    sqlite3_bind_text(stmt, 1, userName.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) != SQLITE_ROW)
    {
        sqlite3_finalize(stmt);
        sendErrorResponse(res, http::status::bad_request,
                          "User does not exist!", version);
        return;
    }

    const std::string accountID(
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    const std::string storedHash(
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
    sqlite3_finalize(stmt);

    if (storedHash != sha256Hash(password))
    {
        sendErrorResponse(res, http::status::unauthorized, "Invalid password!",
                          version);
        return;
    }

    boost::property_tree::ptree resp;
    resp.put("accountID", accountID);
    resp.put("userName", userName);
    resp.put("message", "Logged in successfully!");
    sendJSONResponse(res, http::status::ok, resp, version);
}

// POST /update { "accountID": "...", "newName": "...", "newPassword": "..." }
void
TetrisDBServer::handleUpdate(const boost::property_tree::ptree& pt,
                             const unsigned int version,
                             http::response<http::string_body>& res)
{
    const std::string accountID = pt.get<std::string>("accountID", "");
    if (accountID.empty())
    {
        sendErrorResponse(res, http::status::bad_request, "Missing accountID",
                          version);
        return;
    }

    const std::string newName = pt.get<std::string>("newName", "");
    const std::string newPassword = pt.get<std::string>("newPassword", "");

    std::lock_guard lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    if (!newName.empty())
    {
        const auto checkSql =
            "SELECT accountID FROM players WHERE userName = ?;";
        if (sqlite3_prepare_v2(db_, checkSql, -1, &stmt, nullptr) != SQLITE_OK)
        {
            sendErrorResponse(res, http::status::internal_server_error,
                              "DB error", version);
            return;
        }

        sqlite3_bind_text(stmt, 1, newName.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            sqlite3_finalize(stmt);
            sendErrorResponse(res, http::status::bad_request,
                              "Username already taken!", version);
            return;
        }

        sqlite3_finalize(stmt);

        const auto updateNameSql =
            "UPDATE players SET userName = ? WHERE accountID = ?;";
        if (sqlite3_prepare_v2(db_, updateNameSql, -1, &stmt, nullptr) !=
            SQLITE_OK)
        {
            sendErrorResponse(res, http::status::internal_server_error,
                              "DB error", version);
            return;
        }

        sqlite3_bind_text(stmt, 1, newName.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, accountID.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) != SQLITE_DONE)
        {
            sqlite3_finalize(stmt);
            sendErrorResponse(res, http::status::internal_server_error,
                              "Failed to update userName", version);
            return;
        }

        sqlite3_finalize(stmt);
    }

    if (!newPassword.empty())
    {
        const auto updatePassSql =
            "UPDATE players SET hashedPassword = ? WHERE accountID = ?;";
        if (sqlite3_prepare_v2(db_, updatePassSql, -1, &stmt, nullptr) !=
            SQLITE_OK)
        {
            sendErrorResponse(res, http::status::internal_server_error,
                              "DB error", version);
            return;
        }

        const std::string newHash = sha256Hash(newPassword);
        sqlite3_bind_text(stmt, 1, newHash.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, accountID.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) != SQLITE_DONE)
        {
            sqlite3_finalize(stmt);
            sendErrorResponse(res, http::status::internal_server_error,
                              "Failed to update password", version);
            return;
        }

        sqlite3_finalize(stmt);
    }

    const auto fetchSql = "SELECT userName FROM players WHERE accountID = ?;";
    if (sqlite3_prepare_v2(db_, fetchSql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sendErrorResponse(res, http::status::internal_server_error, "DB error",
                          version);
        return;
    }

    sqlite3_bind_text(stmt, 1, accountID.c_str(), -1, SQLITE_STATIC);
    std::string finalName;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        finalName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    }

    sqlite3_finalize(stmt);

    boost::property_tree::ptree resp;
    resp.put("accountID", accountID);
    resp.put("userName", finalName);
    sendJSONResponse(res, http::status::ok, resp, version);
}

// POST /post_score { "accountID": "...", "score": <number> }
void
TetrisDBServer::handlePostScore(const boost::property_tree::ptree& pt,
                                const unsigned int version,
                                http::response<http::string_body>& res)
{
    const std::string accountID = pt.get<std::string>("accountID", "");
    const int score = pt.get("score", 0);
    if (accountID.empty())
    {
        sendErrorResponse(res, http::status::bad_request, "Missing accountID!",
                          version);
        return;
    }

    std::lock_guard lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    const auto sql = "UPDATE players SET bestScore = ? WHERE accountID = ?;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sendErrorResponse(res, http::status::internal_server_error, "DB error",
                          version);
        return;
    }

    sqlite3_bind_int(stmt, 1, score);
    sqlite3_bind_text(stmt, 2, accountID.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        sendErrorResponse(res, http::status::internal_server_error,
                          "Failed to update score", version);
        return;
    }

    sqlite3_finalize(stmt);

    boost::property_tree::ptree resp;
    resp.put("message", "Score updated successfully");
    sendJSONResponse(res, http::status::ok, resp, version);
}

// GET /get_leaderboard?limit=...
void
TetrisDBServer::handleGetLeaderboard(const unsigned int version,
                                     http::response<http::string_body>& res,
                                     const std::string& query)
{
    int limit = 10;
    auto params = parseQuery(query.substr(query.find('?') + 1));
    if (params.contains("limit"))
    {
        limit = std::stoi(params["limit"]);
    }

    std::lock_guard lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    std::string sql =
        "SELECT accountID, userName, bestScore FROM players ORDER BY bestScore "
        "DESC LIMIT " +
        std::to_string(limit) + ";";
    if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        sendErrorResponse(res, http::status::internal_server_error, "DB error",
                          version);
        return;
    }

    boost::property_tree::ptree leaderboard;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        boost::property_tree::ptree entry;
        entry.put("accountID",
                  reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        entry.put("userName",
                  reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        entry.put("bestScore", sqlite3_column_int(stmt, 2));
        leaderboard.push_back(std::make_pair("", entry));
    }

    sqlite3_finalize(stmt);

    boost::property_tree::ptree root;
    root.add_child("leaderboard", leaderboard);
    sendJSONResponse(res, http::status::ok, root, version);
}

// GET /get_player?accountID=...
void
TetrisDBServer::handleGetPlayer(const std::string& accountID,
                                const unsigned int version,
                                http::response<http::string_body>& res)
{
    std::lock_guard lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;

    // Query basic player info (without friendList and pendingFriendRequests)
    const auto sql =
        "SELECT accountID, userName, bestScore FROM players WHERE accountID = "
        "?;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sendErrorResponse(res, http::status::internal_server_error, "DB error",
                          version);
        return;
    }

    sqlite3_bind_text(stmt, 1, accountID.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_ROW)
    {
        sqlite3_finalize(stmt);
        boost::property_tree::ptree err;
        err.put("error", "Account does not exist");
        sendJSONResponse(res, http::status::bad_request, err, version);
        return;
    }

    boost::property_tree::ptree pt;
    pt.put("accountID",
           reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    pt.put("userName",
           reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
    pt.put("bestScore", sqlite3_column_int(stmt, 2));
    sqlite3_finalize(stmt);

    // Query friend list from the 'friends' table
    auto fetchFriendsSql = "SELECT friendID FROM friends WHERE accountID = ?;";
    if (sqlite3_prepare_v2(db_, fetchFriendsSql, -1, &stmt, nullptr) ==
        SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, accountID.c_str(), -1, SQLITE_STATIC);
        boost::property_tree::ptree friendsPT;
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            boost::property_tree::ptree friendEntry;
            friendEntry.put("", reinterpret_cast<const char*>(
                                    sqlite3_column_text(stmt, 0)));
            friendsPT.push_back(std::make_pair("", friendEntry));
        }
        pt.add_child("friendList", friendsPT);
        sqlite3_finalize(stmt);
    }

    // Query pending friend requests from the 'friend_requests' table
    auto fetchRequestsSql =
        "SELECT senderID FROM friend_requests WHERE receiverID = ?;";
    if (sqlite3_prepare_v2(db_, fetchRequestsSql, -1, &stmt, nullptr) ==
        SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, accountID.c_str(), -1, SQLITE_STATIC);
        boost::property_tree::ptree pendingPT;
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            boost::property_tree::ptree requestEntry;
            requestEntry.put("", reinterpret_cast<const char*>(
                                     sqlite3_column_text(stmt, 0)));
            pendingPT.push_back(std::make_pair("", requestEntry));
        }
        pt.add_child("pendingFriendRequests", pendingPT);
        sqlite3_finalize(stmt);
    }

    sendJSONResponse(res, http::status::ok, pt, version);
}

// POST /send_friend_request { "accountID": sender, "otherAccountID": receiver }
void
TetrisDBServer::handleSendFriendRequest(const boost::property_tree::ptree& pt,
                                        const unsigned int version,
                                        http::response<http::string_body>& res)
{
    const std::string sender = pt.get<std::string>("accountID", "");
    const std::string receiver = pt.get<std::string>("otherAccountID", "");
    if (sender.empty() || receiver.empty() || sender == receiver)
    {
        sendErrorResponse(
            res, http::status::bad_request,
            "Invalid friend request: sender and receiver must be different",
            version);
        return;
    }

    std::lock_guard lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    auto sql =
        "INSERT INTO friend_requests (senderID, receiverID) VALUES (?, ?);";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sendErrorResponse(res, http::status::internal_server_error, "DB error",
                          version);
        return;
    }

    sqlite3_bind_text(stmt, 1, sender.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, receiver.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        sendErrorResponse(res, http::status::internal_server_error,
                          "Failed to send friend request", version);
        return;
    }
    sqlite3_finalize(stmt);

    boost::property_tree::ptree resp;
    resp.put("message", "Friend request sent");
    sendJSONResponse(res, http::status::ok, resp, version);
}

// POST /accept_friend_request { "accountID": receiver, "otherAccountID": sender
// }
void
TetrisDBServer::handleAcceptFriendRequest(
    const boost::property_tree::ptree& pt, const unsigned int version,
    http::response<http::string_body>& res)
{
    const std::string receiver = pt.get<std::string>("accountID", "");
    const std::string sender = pt.get<std::string>("otherAccountID", "");
    if (receiver.empty() || sender.empty() || receiver == sender)
    {
        sendErrorResponse(res, http::status::bad_request,
                          "Invalid friend acceptance", version);
        return;
    }

    std::lock_guard lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;

    // Remove the friend request from friend_requests table
    auto removeSql =
        "DELETE FROM friend_requests WHERE senderID = ? AND receiverID = ?;";
    if (sqlite3_prepare_v2(db_, removeSql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sendErrorResponse(res, http::status::internal_server_error, "DB error",
                          version);
        return;
    }

    sqlite3_bind_text(stmt, 1, sender.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, receiver.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        sendErrorResponse(res, http::status::internal_server_error,
                          "Failed to remove friend request", version);
        return;
    }
    sqlite3_finalize(stmt);

    // Insert two rows into the friends table for bidirectional friendship
    auto insertSql =
        "INSERT INTO friends (accountID, friendID) VALUES (?, ?), (?, ?);";
    if (sqlite3_prepare_v2(db_, insertSql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sendErrorResponse(res, http::status::internal_server_error, "DB error",
                          version);
        return;
    }

    sqlite3_bind_text(stmt, 1, receiver.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, sender.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, sender.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, receiver.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        sendErrorResponse(res, http::status::internal_server_error,
                          "Failed to update friend list", version);
        return;
    }
    sqlite3_finalize(stmt);

    boost::property_tree::ptree resp;
    resp.put("message", "Friend request accepted");
    sendJSONResponse(res, http::status::ok, resp, version);
}

// POST /decline_friend_request { "accountID": receiver, "otherAccountID":
// sender }
void
TetrisDBServer::handleDeclineFriendRequest(
    const boost::property_tree::ptree& pt, const unsigned int version,
    http::response<http::string_body>& res)
{
    const std::string receiver = pt.get<std::string>("accountID", "");
    const std::string sender = pt.get<std::string>("otherAccountID", "");
    if (receiver.empty() || sender.empty() || receiver == sender)
    {
        sendErrorResponse(res, http::status::bad_request,
                          "Invalid decline request", version);
        return;
    }

    std::lock_guard lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    auto sql =
        "DELETE FROM friend_requests WHERE senderID = ? AND receiverID = ?;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sendErrorResponse(res, http::status::internal_server_error, "DB error",
                          version);
        return;
    }

    sqlite3_bind_text(stmt, 1, sender.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, receiver.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        sendErrorResponse(res, http::status::internal_server_error,
                          "Failed to decline friend request", version);
        return;
    }
    sqlite3_finalize(stmt);

    boost::property_tree::ptree resp;
    resp.put("message", "Friend request declined");
    sendJSONResponse(res, http::status::ok, resp, version);
}

// POST /remove_friend { "accountID": user1, "otherAccountID": user2 }
void
TetrisDBServer::handleRemoveFriend(const boost::property_tree::ptree& pt,
                                   const unsigned int version,
                                   http::response<http::string_body>& res)
{
    const std::string user1 = pt.get<std::string>("accountID", "");
    const std::string user2 = pt.get<std::string>("otherAccountID", "");
    if (user1.empty() || user2.empty() || user1 == user2)
    {
        sendErrorResponse(res, http::status::bad_request,
                          "Invalid friend removal", version);
        return;
    }

    std::lock_guard lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    auto sql = "DELETE FROM friends WHERE (accountID = ? AND friendID = ?) OR "
               "(accountID = ? AND friendID "
               "= ?);";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sendErrorResponse(res, http::status::internal_server_error, "DB error",
                          version);
        return;
    }

    sqlite3_bind_text(stmt, 1, user1.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, user2.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, user2.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, user1.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        sendErrorResponse(res, http::status::internal_server_error,
                          "Failed to remove friend", version);
        return;
    }
    sqlite3_finalize(stmt);

    boost::property_tree::ptree resp;
    resp.put("message", "Friend removed successfully");
    sendJSONResponse(res, http::status::ok, resp, version);
}

// GET /get_messages?accountID=...&otherAccountID=...
void
TetrisDBServer::handleGetMessages(const std::string& query,
                                  const unsigned int version,
                                  http::response<http::string_body>& res)
{
    auto params = parseQuery(query.substr(query.find('?') + 1));
    if (!params.contains("accountID") || !params.contains("otherAccountID"))
    {
        sendErrorResponse(res, http::status::bad_request, "Missing parameters",
                          version);
        return;
    }
    const std::string user1 = params["accountID"];
    const std::string user2 = params["otherAccountID"];

    std::lock_guard lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    const auto sql =
        "SELECT messageID, senderID, receiverID, content, timestamp FROM "
        "messages "
        "WHERE (senderID = ? AND receiverID = ?) OR (senderID = ? AND "
        "receiverID = ?) "
        "ORDER BY timestamp DESC;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sendErrorResponse(res, http::status::internal_server_error, "DB error",
                          version);
        return;
    }

    sqlite3_bind_text(stmt, 1, user1.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, user2.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, user2.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, user1.c_str(), -1, SQLITE_STATIC);
    boost::property_tree::ptree messages;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        boost::property_tree::ptree message;
        message.put("messageID", reinterpret_cast<const char*>(
                                     sqlite3_column_text(stmt, 0)));
        message.put("senderID", reinterpret_cast<const char*>(
                                    sqlite3_column_text(stmt, 1)));
        message.put("receiverID", reinterpret_cast<const char*>(
                                      sqlite3_column_text(stmt, 2)));
        message.put("content", reinterpret_cast<const char*>(
                                   sqlite3_column_text(stmt, 3)));
        message.put("timestamp", reinterpret_cast<const char*>(
                                     sqlite3_column_text(stmt, 4)));
        messages.push_back(std::make_pair("", message));
    }
    sqlite3_finalize(stmt);

    boost::property_tree::ptree root;
    root.add_child("messages", messages);
    sendJSONResponse(res, http::status::ok, root, version);
}

// POST /post_message { "accountID": sender, "otherAccountID": receiver,
// "messageContent": "..." }
void
TetrisDBServer::handlePostMessage(const boost::property_tree::ptree& pt,
                                  const unsigned int version,
                                  http::response<http::string_body>& res)
{
    const std::string sender = pt.get<std::string>("accountID", "");
    const std::string receiver = pt.get<std::string>("otherAccountID", "");
    const std::string content = pt.get<std::string>("messageContent", "");
    if (sender.empty() || receiver.empty() || content.empty())
    {
        sendErrorResponse(res, http::status::bad_request, "Missing parameters",
                          version);
        return;
    }

    const std::string messageID = generateUUID();

    std::lock_guard lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    const auto sql =
        "INSERT INTO messages (messageID, senderID, receiverID, content) "
        "VALUES (?, ?, ?, ?);";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sendErrorResponse(res, http::status::internal_server_error, "DB error",
                          version);
        return;
    }

    sqlite3_bind_text(stmt, 1, messageID.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, sender.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, receiver.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, content.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        sendErrorResponse(res, http::status::internal_server_error,
                          "Failed to post message", version);
        return;
    }
    sqlite3_finalize(stmt);

    boost::property_tree::ptree resp;
    resp.put("message", "Message posted successfully");
    sendJSONResponse(res, http::status::ok, resp, version);
}

// POST /delete_message { "messageID": "..." }
void
TetrisDBServer::handleDeleteMessage(const boost::property_tree::ptree& pt,
                                    const unsigned int version,
                                    http::response<http::string_body>& res)
{
    const std::string messageID = pt.get<std::string>("messageID", "");
    if (messageID.empty())
    {
        sendErrorResponse(res, http::status::bad_request, "Missing messageID",
                          version);
        return;
    }

    std::lock_guard lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    const auto sql = "DELETE FROM messages WHERE messageID = ?;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sendErrorResponse(res, http::status::internal_server_error, "DB error",
                          version);
        return;
    }

    sqlite3_bind_text(stmt, 1, messageID.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        sendErrorResponse(res, http::status::internal_server_error,
                          "Failed to delete message", version);
        return;
    }
    sqlite3_finalize(stmt);

    boost::property_tree::ptree resp;
    resp.put("message", "Message deleted successfully");
    sendJSONResponse(res, http::status::ok, resp, version);
}
