#include "ClientSession.hpp"

ClientSession::ClientSession(const std::string &server_ip, int lobby_port,
                             int db_port, bool debug)
    : dbRequestManager(server_ip, db_port),
      gameRequestManager(server_ip, lobby_port), debug_(debug) {
    // this is the constructor of the ClientSession class
    // it will initialize the DBRequestManager and the GameRequestManager

    if (gameRequestManager.connectToServer() != StatusCode::SUCCESS) {
        throw std::runtime_error("[err] Failed to connect to game server.");
    }
}

ClientSession::ClientSession(Config &config, bool debug)
    : dbRequestManager(config.get("server_ip"), std::stoi(config.get("db_port"))),
      gameRequestManager(config.get("server_ip"), std::stoi(config.get("lobby_port"))),
      debug_(debug) {

    // this is the constructor of the ClientSession class
    // it will initialize the DBRequestManager and the GameRequestManager

    if (gameRequestManager.connectToServer() != StatusCode::SUCCESS) {
        throw std::runtime_error("[err] Failed to connect to game server.");
    }

}

ClientSession::~ClientSession() {
    // this is the destructor of the ClientSession class
    // I really doubt that we need to do anything here
    // but I will leave it here just in case
}

std::string
ClientSession::getServerIP() {
    // this method is used to get the server IP
    // it will return the server IP
    return gameRequestManager.getServerIP();
}

int
ClientSession::getLobbyPort() const {
    // this method is used to get the lobby port
    // it will return the lobby port
    return gameRequestManager.getPort();
}

int
ClientSession::getDBPort() const {
    // this method is used to get the DB port
    // it will return the DB port
    return dbRequestManager.getPort();
}

std::string
ClientSession::getUsername() {
    return username_;
}

std::string
ClientSession::getAccountID() {
    return accountID_;
}

std::string
ClientSession::getToken() {
    return token_;
}

int
ClientSession::getBestScore() const {
    return bestScore_;
}

std::vector<std::string> &
ClientSession::getFriendList() {
    return friendList_;
}

std::vector<std::string> &
ClientSession::getPendingFriendRequests() {
    return pendingFriendRequests_;
}

void
ClientSession::setUsername(const std::string &username) {
    username_ = username;
}

void
ClientSession::setAccountID(const std::string &accountID) {
    accountID_ = accountID;
}

void
ClientSession::setToken(const std::string &token) {
    token_ = token;
}

void
ClientSession::setBestScore(const int score) {
    bestScore_ = score;
}

void
ClientSession::updateLocalMessages(const std::string &otherAccountID, const ChatMessage &message) {
    // Add the new message to our local cache
    // We'll use a map to store conversations by the other user's account ID
    if (!conversations_.contains(otherAccountID)) {
        conversations_[otherAccountID] = std::vector<ChatMessage>();
    }

    conversations_[otherAccountID].push_back(message);
}

void
ClientSession::setFriendList(const std::vector<std::string> &friends) {
    friendList_ = friends;
}

void
ClientSession::setPendingFriendRequests(
    const std::vector<std::string> &requests) {
    pendingFriendRequests_ = requests;
}

StatusCode
ClientSession::loginPlayer(const std::string &username,
                           const std::string &password) {
    const DBResponse response =
            dbRequestManager.loginPlayer(username, password);

    if (response.status == 200) {
        setAccountID(response.json.get<std::string>("accountID"));
        setUsername(response.json.get<std::string>("userName"));
        // std::cout << "Login successful. AccountID: " << getAccountID() << std::endl;
        return StatusCode::SUCCESS;
    } else {
        std::cerr << "Login error (" << response.status << "): "
                << response.json.get<std::string>("error", "Unknown error")
                << std::endl;
        return StatusCode::ERROR_LOGGING_IN;
    }
}

StatusCode
ClientSession::registerPlayer(const std::string &username,
                              const std::string &password) {
    const DBResponse response =
            dbRequestManager.registerPlayer(username, password);

    if (response.status == 200) {
        setAccountID(response.json.get<std::string>("accountID"));
        setUsername(response.json.get<std::string>("userName"));
        // std::cout << "Registration successful. AccountID: " << getAccountID() << std::endl;
        return StatusCode::SUCCESS;
    }
    if (response.status == 400) {
        // Bad request
        std::cerr << "Registration error (" << response.status << "): "
                << response.json.get<std::string>("error", "Unknown error")
                << std::endl;
        return StatusCode::ERROR_USERNAME_TAKEN;
    }
    std::cerr << "Registration error (" << response.status << "): "
            << response.json.get<std::string>("error", "Unknown error")
            << std::endl;
    return StatusCode::ERROR_REGISTERING;
}

std::vector<PlayerScore>
ClientSession::getLeaderboard(const int limit) const {
    std::vector<PlayerScore> leaderboard;

    // Request the leaderboard from the database server
    const DBResponse response = dbRequestManager.getLeaderboard(limit);

    // Check if the response was successful
    if (response.status == 200) {
        try {
            // Access the leaderboard array from the response
            const auto &leaderboardArray = response.json.get_child("leaderboard");

            // Iterate through each player in the leaderboard
            int rank = 1;
            for (const auto &item: leaderboardArray) {
                const auto &player = item.second;

                // Extract player data
                PlayerScore score;
                score.rank = rank++;
                score.name = player.get<std::string>("userName", "Unknown");
                score.score = player.get<int>("bestScore", 0);

                // Add to the leaderboard vector
                leaderboard.push_back(score);
            }

            // std::cout << "Fetched leaderboard with " << leaderboard.size() << " entries" << std::endl;
        } catch (std::exception &e) {
            std::cerr << "Error parsing leaderboard data: " << e.what() << std::endl;
        }
    } else {
        std::cerr << "Error fetching leaderboard (" << response.status << "): "
                << response.json.get<std::string>("error", "Unknown error")
                << std::endl;
    }

    return leaderboard;
}

std::vector<ChatMessage>
ClientSession::getPlayerMessages(const std::string &otherAccountID) {
    // Check if we have a valid accountID
    if (getAccountID().empty()) {
        std::cerr << "No accountID set; cannot fetch messages." << std::endl;
        return std::vector<ChatMessage>();
    }

    std::vector<ChatMessage> messages;
    const DBResponse response = dbRequestManager.getMessages(getAccountID(), otherAccountID);

    // Check if the response was successful
    if (response.status == 200) {
        try {
            // Access the messages array from the response
            const auto &messagesArray = response.json.get_child("messages");

            // Iterate through each message
            for (const auto &item: messagesArray) {
                const auto &message = item.second;

                ChatMessage chatMsg;
                const std::string senderId = message.get<std::string>("senderID", "");

                // Set the "from" field based on whether this user sent the message
                if (senderId == getAccountID()) {
                    chatMsg.from = "Me"; // Message sent by this user
                } else {
                    // Try to get username from message or use a default
                    // turn id into username
                    chatMsg.from = getFriendUsername(senderId);
                }

                // Extract message content
                chatMsg.text = message.get<std::string>("content", "");

                // Cache the message locally
                updateLocalMessages(otherAccountID, chatMsg);

                // Add to the messages vector
                messages.push_back(chatMsg);
            }
            // std::cout << "Fetched " << messages.size() << " messages with " << otherAccountID << std::endl;
        } catch (std::exception &e) {
            std::cerr << "Error parsing messages data: " << e.what() << std::endl;
        }
    } else {
        std::cerr << "Error fetching messages (" << response.status << "): "
                << response.json.get<std::string>("error", "Unknown error")
                << std::endl;
    }

    return messages;
}

std::string
ClientSession::getAccountIDFromUsername(const std::string &username) const {
    std::cout << "[ClientSession] Looking up accountID for username: " << username << std::endl;

    // Send request to get account ID by username
    const DBResponse response = dbRequestManager.getAccountIDByUsername(username);

    if (response.status == 200) {
        try {
            const std::string accountID = response.json.get<std::string>("accountID", "");
            std::cout << "[ClientSession] Found accountID: " << accountID << " for username: " << username << std::endl;
            return accountID;
        } catch (const std::exception &e) {
            std::cerr << "[ClientSession] Error parsing account ID response: " << e.what() << std::endl;
        }
    } else {
        std::cerr << "[ClientSession] Failed to get accountID for username " << username
                << " (status: " << response.status << ")" << std::endl;
    }

    return ""; // Return empty string on failure
}

std::string
ClientSession::getFriendUsername(const std::string &friendID) {
    if (friendIDToUsername.contains(friendID)) {
        return friendIDToUsername[friendID];
    }
    return "Unknown";
}

std::string
ClientSession::getRequestUsername(const std::string &requestID) {
    if (pendingRequestIDToUsername.contains(requestID)) {
        return pendingRequestIDToUsername[requestID];
    }
    return "Unknown";
}

StatusCode
ClientSession::fetchPlayerData() {
    if (getAccountID().empty()) {
        std::cerr << "No accountID set; cannot fetch player data." << std::endl;
        return StatusCode::ERROR_NO_SESSION_TOKEN;
    }

    std::cout << "[ClientSession] Fetching data for accountID: " << getAccountID() << std::endl;
    DBResponse response = dbRequestManager.getPlayer(getAccountID());

    if (response.status == 200) {
        try {
            // Update basic fields
            setUsername(response.json.get<std::string>("userName", username_));
            setBestScore(response.json.get<int>("bestScore", 0));

            // Print raw JSON for debugging
            std::ostringstream oss;
            write_json(oss, response.json, false);
            std::cout << "[ClientSession] Received JSON: " << oss.str() << std::endl;

            // Clear existing maps
            friendIDToUsername.clear();
            pendingRequestIDToUsername.clear();

            // Parse friendList array
            std::vector<std::string> friends;
            try {
                if (response.json.find("friendList") != response.json.not_found()) {
                    const auto &friendsArray = response.json.get_child("friendList");
                    for (const auto &child: friendsArray) {
                        std::string friendID = child.second.data();
                        friends.push_back(friendID);

                        // Fetch and store the username for this friend ID
                        DBResponse nameResponse = dbRequestManager.getUsernameByAccountID(friendID);
                        if (nameResponse.status == 200) {
                            std::string username = nameResponse.json.get<std::string>("username", "Unknown");
                            friendIDToUsername[friendID] = username;
                            std::cout << "[ClientSession] Added friend: " << username
                                    << " (ID: " << friendID << ")" << std::endl;
                        } else {
                            friendIDToUsername[friendID] = "Unknown";
                            std::cout << "[ClientSession] Added friend with unknown username"
                                    << " (ID: " << friendID << ")" << std::endl;
                        }
                    }
                }
            } catch (const std::exception &e) {
                std::cerr << "[ClientSession] Error parsing friend list: " << e.what() << std::endl;
            }
            setFriendList(friends);

            // Parse pendingFriendRequests array
            std::vector<std::string> pending;
            try {
                if (response.json.find("pendingFriendRequests") != response.json.not_found()) {
                    const auto &pendingArray = response.json.get_child("pendingFriendRequests");
                    for (const auto &child: pendingArray) {
                        std::string requestID = child.second.data();
                        pending.push_back(requestID);

                        // Fetch and store the username for this friend request ID
                        DBResponse nameResponse = dbRequestManager.getUsernameByAccountID(requestID);
                        if (nameResponse.status == 200) {
                            std::string username = nameResponse.json.get<std::string>("username", "Unknown");
                            pendingRequestIDToUsername[requestID] = username;
                            std::cout << "[ClientSession] Added pending request: " << username
                                    << " (ID: " << requestID << ")" << std::endl;
                        } else {
                            pendingRequestIDToUsername[requestID] = "Unknown";
                            std::cout << "[ClientSession] Added pending request with unknown username"
                                    << " (ID: " << requestID << ")" << std::endl;
                        }
                    }
                }
            } catch (const std::exception &e) {
                std::cerr << "[ClientSession] Error parsing pending requests: " << e.what() << std::endl;
            }
            setPendingFriendRequests(pending);

            return StatusCode::SUCCESS;
        } catch (const std::exception &e) {
            std::cerr << "[ClientSession] Error parsing player data: " << e.what() << std::endl;
            return StatusCode::ERROR;
        }
    }

    std::cerr << "[ClientSession] Error fetching player data (" << response.status << ")" << std::endl;
    return StatusCode::ERROR;
}

StatusCode
ClientSession::updatePlayer(const std::string &newName,
                            const std::string &newPassword) {
    if (getAccountID().empty()) {
        std::cerr << "No accountID set; cannot update player." << std::endl;
        return StatusCode::ERROR_NO_SESSION_TOKEN;
    }

    const DBResponse response =
            dbRequestManager.updatePlayer(getAccountID(), newName, newPassword);

    if (response.status == 200) {
        setUsername(response.json.get<std::string>("userName", username_));
        return StatusCode::SUCCESS;
    }
    std::cerr << "Error updating player (" << response.status << "): "
            << response.json.get<std::string>("error", "Unknown error")
            << std::endl;
    return StatusCode::ERROR;
}

StatusCode
ClientSession::postScore(int score) {
    if (getAccountID().empty()) {
        std::cerr << "No accountID set; cannot post score." << std::endl;
        return StatusCode::ERROR_NO_SESSION_TOKEN;
    }

    const DBResponse response =
            dbRequestManager.postPlayerScore(getAccountID(), score);

    if (response.status == 200) {
        // Refresh local data after score update
        (void) fetchPlayerData();
        return StatusCode::SUCCESS;
    }
    std::cerr << "Error posting score (" << response.status << "): "
            << response.json.get<std::string>("error", "Unknown error")
            << std::endl;
    return StatusCode::ERROR;
}

StatusCode
ClientSession::sendMessage(const std::string &receiverID, const std::string &messageContent) {
    if (getAccountID().empty()) {
        std::cerr << "No accountID set; cannot send message." << std::endl;
        return StatusCode::ERROR_NO_SESSION_TOKEN;
    }

    if (messageContent.empty()) {
        std::cerr << "Cannot send empty message." << std::endl;
        return StatusCode::ERROR_INVALID_MESSAGE;
    }

    const DBResponse response =
            dbRequestManager.postMessage(getAccountID(), receiverID, messageContent);

    if (response.status == 200) {
        // std::cout << "Message sent to " << receiverID << std::endl;

        // Add the message to our local cache for this conversation
        ChatMessage newMessage;
        newMessage.from = "Me";
        newMessage.text = messageContent;

        // Store this message in our local cache
        updateLocalMessages(receiverID, newMessage);

        return StatusCode::SUCCESS;
    }
    std::cerr << "Error sending message (" << response.status << "): "
            << response.json.get<std::string>("error", "Unknown error")
            << std::endl;
    return StatusCode::ERROR_SENDING_MESSAGE;
}

StatusCode
ClientSession::sendFriendRequest(const std::string &receiverIdentifier) {
    if (getAccountID().empty()) {
        std::cerr << "No accountID set; cannot send friend request." << std::endl;
        return StatusCode::ERROR_NO_SESSION_TOKEN;
    }

    // Determine if this is a username or an account ID
    // Account IDs are UUID strings (36 chars with dashes)
    std::string receiverAccountID;

    if (receiverIdentifier.length() == 36 && receiverIdentifier.find('-') != std::string::npos) {
        // This looks like an account ID already
        receiverAccountID = receiverIdentifier;
        std::cout << "[ClientSession] Using provided accountID: " << receiverAccountID << std::endl;
    } else {
        // This is probably a username, so we need to look up the account ID
        receiverAccountID = getAccountIDFromUsername(receiverIdentifier);
        if (receiverAccountID.empty()) {
            std::cerr << "[ClientSession] Could not find accountID for username: " << receiverIdentifier << std::endl;
            return StatusCode::ERROR_USER_NOT_FOUND;
        }
    }

    // Now we can send the friend request with the proper account ID
    std::cout << "[ClientSession] Sending friend request to accountID: " << receiverAccountID << std::endl;
    const DBResponse response = dbRequestManager.sendFriendRequest(getAccountID(), receiverAccountID);

    if (response.status == 200) {
        std::cout << "[ClientSession] Friend request sent successfully. Refreshing data..." << std::endl;
        return fetchPlayerData(); // Refresh data after sending request
    }

    std::cerr << "[ClientSession] Error sending friend request (" << response.status << ")" << std::endl;
    return StatusCode::ERROR;
}

StatusCode
ClientSession::acceptFriendRequest(const std::string &senderID) {
    if (getAccountID().empty()) {
        std::cerr << "No accountID set; cannot accept friend request."
                << std::endl;
        return StatusCode::ERROR_NO_SESSION_TOKEN;
    }

    DBResponse response =
            dbRequestManager.acceptFriendRequest(getAccountID(), senderID);

    if (response.status == 200) {
        // Optionally, update local friend list
        (void) fetchPlayerData();
        return StatusCode::SUCCESS;
    }
    std::cerr << "Error accepting friend request (" << response.status
            << "): "
            << response.json.get<std::string>("error", "Unknown error")
            << std::endl;
    return StatusCode::ERROR;
}

StatusCode
ClientSession::declineFriendRequest(const std::string &senderID) {
    if (getAccountID().empty()) {
        std::cerr << "No accountID set; cannot decline friend request."
                << std::endl;
        return StatusCode::ERROR_NO_SESSION_TOKEN;
    }

    DBResponse response =
            dbRequestManager.declineFriendRequest(getAccountID(), senderID);

    if (response.status == 200) {
        // Optionally, update local pending list
        (void) fetchPlayerData();
        return StatusCode::SUCCESS;
    }
    std::cerr << "Error declining friend request (" << response.status
            << "): "
            << response.json.get<std::string>("error", "Unknown error")
            << std::endl;
    return StatusCode::ERROR;
}

StatusCode
ClientSession::removeFriend(const std::string &friendID) {
    if (getAccountID().empty()) {
        std::cerr << "No accountID set; cannot remove friend." << std::endl;
        return StatusCode::ERROR_NO_SESSION_TOKEN;
    }

    const DBResponse response =
            dbRequestManager.removeFriend(getAccountID(), friendID);

    if (response.status == 200) {
        // Optionally, update local friend list
        (void) fetchPlayerData();
        return StatusCode::SUCCESS;
    }
    std::cerr << "Error removing friend (" << response.status << "): "
            << response.json.get<std::string>("error", "Unknown error")
            << std::endl;
    return StatusCode::ERROR;
}

// ================== Game operations ================== //

ClientStatus
ClientSession::getOwnStatus() {
    // this method is used to get the status of the client
    // it will return the status of the client
    return getPlayerStatus(getUsername());
}

ClientStatus
ClientSession::getPlayerStatus(const std::string &username) {
    ServerResponse response =
            this->gameRequestManager.getPlayerStatus(username);

    // we have to check if the response was successful
    if (response.status != StatusCode::SUCCESS) {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                << std::endl;
        return ClientStatus::OFFLINE;
    }

    return static_cast<ClientStatus>(std::stoi(response.data.at("status")));
}

StatusCode
ClientSession::startSession() {
    ServerResponse response =
            this->gameRequestManager.startSession(getUsername());

    // we have to check if the response was successful
    if (response.status == StatusCode::SUCCESS_REPLACED_SESSION ||
        response.status == StatusCode::SUCCESS) {
        this->setToken(response.data.at("token"));
        return StatusCode::SUCCESS;
    }
    std::cerr << "Error: " << getStatusCodeString(response.status)
            << std::endl;
    return response.status;
}

StatusCode
ClientSession::endSession() {
    ServerResponse response = this->gameRequestManager.endSession(getToken());

    // we have to check if the response was successful
    if (response.status == StatusCode::SUCCESS) {
        // we set the token of the client
        this->setToken("");
        return StatusCode::SUCCESS;
    }
    std::cerr << "Error: " << getStatusCodeString(response.status)
            << std::endl;
    return response.status;
}

std::unordered_map<std::string, std::string>
ClientSession::getPublicLobbiesList() {
    ServerResponse response = this->gameRequestManager.getPublicLobbiesList();

    // we have to check if the response was successful
    if (response.status != StatusCode::SUCCESS) {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                << std::endl;
        return {};
    }

    // we return the list of lobbies
    return response.data;
}

StatusCode
ClientSession::createAndJoinLobby(GameMode gameMode, int maxPlayers,
                                  bool isPublic) {
    ServerResponse response = this->gameRequestManager.createAndJoinLobby(
        getToken(), gameMode, maxPlayers, isPublic);

    // we have to check if the response was successful
    if (response.status != StatusCode::SUCCESS) {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                << std::endl;
        return response.status;
    }
    return StatusCode::SUCCESS;
}

StatusCode
ClientSession::joinLobby(const std::string &lobbyID) {
    ServerResponse response =
            this->gameRequestManager.joinLobby(getToken(), lobbyID);

    // we have to check if the response was successful
    if (response.status != StatusCode::SUCCESS) {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                << std::endl;
        return response.status;
    }

    return StatusCode::SUCCESS;
}

StatusCode
ClientSession::spectateLobby(const std::string &lobbyID) {
    ServerResponse response =
            this->gameRequestManager.spectateLobby(getToken(), lobbyID);

    // we have to check if the response was successful
    if (response.status != StatusCode::SUCCESS) {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                << std::endl;
        return response.status;
    }

    return StatusCode::SUCCESS;
}

LobbyState
ClientSession::getCurrentLobbyState() {
    ServerResponse response =
            this->gameRequestManager.getCurrentLobbyState(getToken());

    // we have to check if the response was successful
    if (response.status != StatusCode::SUCCESS) {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                << std::endl;
        return LobbyState::generateEmptyState();
    }

    LobbyState lobbyState =
            LobbyState::deserialize(response.data.at("lobby"));
    return lobbyState;
}

StatusCode
ClientSession::leaveLobby() {
    ServerResponse response = this->gameRequestManager.leaveLobby(getToken());

    // we have to check if the response was successful
    if (response.status != StatusCode::SUCCESS) {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                << std::endl;
        return response.status;
    }

    return StatusCode::SUCCESS;
}

StatusCode
ClientSession::readyUp() {
    ServerResponse response = this->gameRequestManager.readyUp(getToken());

    // we have to check if the response was successful
    if (response.status != StatusCode::SUCCESS) {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                << std::endl;
        return response.status;
    }

    return StatusCode::SUCCESS;
}

StatusCode
ClientSession::unreadyUp() {
    ServerResponse response = this->gameRequestManager.unreadyUp(getToken());

    // we have to check if the response was successful
    if (response.status != StatusCode::SUCCESS) {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                << std::endl;
        return response.status;
    }

    return StatusCode::SUCCESS;
}

StatusCode
ClientSession::sendKeyStroke(const Action &keyStroke) {
    KeyStrokePacket keyStrokePacket;
    keyStrokePacket.token = getToken();
    keyStrokePacket.action = keyStroke;
    ServerResponse response = this->gameRequestManager.sendKeyStroke(
        keyStrokePacket.token, keyStrokePacket);

    // we have to check if the response was successful
    if (response.status != StatusCode::SUCCESS) {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                << std::endl;
        return response.status;
    }

    return StatusCode::SUCCESS;
}

PlayerState
ClientSession::getPlayerState() {
    ServerResponse response = this->gameRequestManager.getGameState(getToken());

    // we have to check if the response was successful
    if (response.status != StatusCode::SUCCESS) {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                << std::endl;
        return PlayerState::generateEmptyState();
    }

    PlayerState playerState =
            PlayerState::deserialize(response.data.at("gamestate"));
    return playerState;
}

SpectatorState
ClientSession::getSpectatorState() {
    ServerResponse response = this->gameRequestManager.getGameState(getToken());

    // we have to check if the response was successful
    if (response.status != StatusCode::SUCCESS) {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                << std::endl;
        return SpectatorState::generateEmptyState();
    }

    SpectatorState spectatorState =
            SpectatorState::deserialize(response.data.at("gamestate"));
    return spectatorState;
}
