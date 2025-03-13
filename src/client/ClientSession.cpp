#include "ClientSession.hpp"

ClientSession::ClientSession(const std::string& server_ip, int lobby_port,
                             int db_port, bool debug)
    : dbRequestManager(server_ip, db_port),
      gameRequestManager(server_ip, lobby_port), debug_(debug)
{

    // this is the constructor of the ClientSession class
    // it will initialize the DBRequestManager and the GameRequestManager

    if (gameRequestManager.connectToServer() != StatusCode::SUCCESS)
    {
        throw std::runtime_error("[err] Failed to connect to game server.");
    }
}

ClientSession::~ClientSession()
{
    // this is the destructor of the ClientSession class
    // I really doubt that we need to do anything here
    // but I will leave it here just in case
}

std::string
ClientSession::getServerIP()
{
    // this method is used to get the server IP
    // it will return the server IP
    return gameRequestManager.getServerIP();
}

int
ClientSession::getLobbyPort()
{
    // this method is used to get the lobby port
    // it will return the lobby port
    return gameRequestManager.getPort();
}

int
ClientSession::getDBPort()
{
    // this method is used to get the DB port
    // it will return the DB port
    return dbRequestManager.getPort();
}

std::string
ClientSession::getUsername()
{
    return username_;
}

std::string
ClientSession::getAccountID()
{
    return accountID_;
}

std::string
ClientSession::getToken()
{
    return token_;
}

int
ClientSession::getBestScore()
{
    return bestScore_;
}

std::vector<std::string>&
ClientSession::getFriendList()
{
    return friendList_;
}

std::vector<std::string>&
ClientSession::getPendingFriendRequests()
{
    return pendingFriendRequests_;
}

void
ClientSession::setUsername(const std::string& username)
{
    username_ = username;
}

void
ClientSession::setAccountID(const std::string& accountID)
{
    accountID_ = accountID;
}

void
ClientSession::setToken(const std::string& token)
{
    token_ = token;
}

void
ClientSession::setBestScore(const int score)
{
    bestScore_ = score;
}

void
ClientSession::updateLocalMessages(const std::string& otherAccountID, const ChatMessage& message)
{
    // Add the new message to our local cache
    // We'll use a map to store conversations by the other user's account ID
    if (!conversations_.contains(otherAccountID)) {
        conversations_[otherAccountID] = std::vector<ChatMessage>();
    }

    conversations_[otherAccountID].push_back(message);
}

void
ClientSession::setFriendList(const std::vector<std::string>& friends)
{
    friendList_ = friends;
}

void
ClientSession::setPendingFriendRequests(
    const std::vector<std::string>& requests)
{
    pendingFriendRequests_ = requests;
}

StatusCode
ClientSession::loginPlayer(const std::string& username,
                           const std::string& password)
{
    const DBResponse response =
        dbRequestManager.loginPlayer(username, password);

    if (response.status == 200)
    {
        setAccountID(response.json.get<std::string>("accountID"));
        setUsername(response.json.get<std::string>("userName"));
        std::cout << "Login successful. AccountID: " << getAccountID() << std::endl;
        return StatusCode::SUCCESS;

    }

    else
    {
        std::cerr << "Login error (" << response.status << "): "
                  << response.json.get<std::string>("error", "Unknown error")
                  << std::endl;
        return StatusCode::ERROR_LOGGING_IN;
    }
}

StatusCode
ClientSession::registerPlayer(const std::string& username,
                              const std::string& password)
{
    const DBResponse response =
        dbRequestManager.registerPlayer(username, password);

    if (response.status == 200)
    {
        setAccountID(response.json.get<std::string>("accountID"));
        setUsername(response.json.get<std::string>("userName"));
        std::cout << "Registration successful. AccountID: " << getAccountID() << std::endl;
        return StatusCode::SUCCESS;
    }
    if (response.status == 400) { // Bad request
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
ClientSession::getLeaderboard(const int limit)
{
    std::vector<PlayerScore> leaderboard;

    // Request the leaderboard from the database server
    const DBResponse response = dbRequestManager.getLeaderboard(limit);

    // Check if the response was successful
    if (response.status == 200)
    {
        try
        {
            // Access the leaderboard array from the response
            const auto& leaderboardArray = response.json.get_child("leaderboard");

            // Iterate through each player in the leaderboard
            int rank = 1;
            for (const auto& item : leaderboardArray)
            {
                const auto& player = item.second;

                // Extract player data
                PlayerScore score;
                score.rank = rank++;
                score.name = player.get<std::string>("userName", "Unknown");
                score.score = player.get<int>("bestScore", 0);

                // Add to the leaderboard vector
                leaderboard.push_back(score);
            }

            std::cout << "Fetched leaderboard with " << leaderboard.size() << " entries" << std::endl;
        }
        catch (std::exception& e)
        {
            std::cerr << "Error parsing leaderboard data: " << e.what() << std::endl;
        }
    }
    else
    {
        std::cerr << "Error fetching leaderboard (" << response.status << "): "
                  << response.json.get<std::string>("error", "Unknown error")
                  << std::endl;
    }

    return leaderboard;
}

std::vector<ChatMessage>
ClientSession::getPlayerMessages(const std::string& otherAccountID)
{
    // Check if we have a valid accountID
    if (getAccountID().empty())
    {
        std::cerr << "No accountID set; cannot fetch messages." << std::endl;
        return std::vector<ChatMessage>();
    }

    // If we already have messages cached, return those
    if (conversations_.find(otherAccountID) != conversations_.end()) {
        return conversations_[otherAccountID];
    }

    // Otherwise, fetch messages from the database server
    std::vector<ChatMessage> messages;
    const DBResponse response = dbRequestManager.getMessages(getAccountID(), otherAccountID);

    // Check if the response was successful
    if (response.status == 200)
    {
        try
        {
            // Access the messages array from the response
            const auto& messagesArray = response.json.get_child("messages");

            // Iterate through each message
            for (const auto& item : messagesArray)
            {
                const auto& message = item.second;

                ChatMessage chatMsg;
                const std::string senderId = message.get<std::string>("senderID", "");

                // Set the "from" field based on whether this user sent the message
                if (senderId == getAccountID()) {
                    chatMsg.from = "Me"; // Message sent by this user
                } else {
                    // Try to get username from message or use a default
                    chatMsg.from = message.get<std::string>("senderName", "User");
                }

                // Extract message content
                chatMsg.text = message.get<std::string>("content", "");

                // Add to the messages vector
                messages.push_back(chatMsg);
            }

            // Cache these messages for future use
            conversations_[otherAccountID] = messages;

            std::cout << "Fetched " << messages.size() << " messages with " << otherAccountID << std::endl;
        }
        catch (std::exception& e)
        {
            std::cerr << "Error parsing messages data: " << e.what() << std::endl;
        }
    }
    else
    {
        std::cerr << "Error fetching messages (" << response.status << "): "
                  << response.json.get<std::string>("error", "Unknown error")
                  << std::endl;
    }

    return messages;
}

void
ClientSession::fetchPlayerData()
{
    if (getAccountID().empty())
    {
        std::cerr << "No accountID set; cannot fetch player data." << std::endl;
        return;
    }

    DBResponse response = dbRequestManager.getPlayer(getAccountID());
    if (response.status == 200)
    {
        // Update basic fields
        setUsername(response.json.get<std::string>("userName", username_));
        setBestScore(response.json.get<int>("bestScore", 0));

        // Parse friendList array
        std::vector<std::string> friends;
        if (const auto maybeFriends =
                response.json.get_child_optional("friendList"))
        {
            for (const auto& child : maybeFriends.get())
            {
                friends.push_back(child.second.data());
            }
        }
        setFriendList(friends);

        // Parse pendingFriendRequests array
        std::vector<std::string> pending;
        if (const auto maybePending =
                response.json.get_child_optional("pendingFriendRequests"))
        {
            for (const auto& child : maybePending.get())
            {
                pending.push_back(child.second.data());
            }
        }
        setPendingFriendRequests(pending);

        std::cout << "Fetched player data for " << getUsername() << std::endl;
    }
    else
    {
        std::cerr << "Error fetching player data (" << response.status << "): "
                  << response.json.get<std::string>("error", "Unknown error")
                  << std::endl;
    }
}

void
ClientSession::updatePlayer(const std::string& newName,
                            const std::string& newPassword)
{
    if (getAccountID().empty())
    {
        std::cerr << "No accountID set; cannot update player." << std::endl;
        return;
    }

    const DBResponse response =
        dbRequestManager.updatePlayer(getAccountID(), newName, newPassword);

    if (response.status == 200)
    {
        setUsername(response.json.get<std::string>("userName", username_));
        std::cout << "Player updated successfully!" << std::endl;
    }
    else
    {
        std::cerr << "Error updating player (" << response.status << "): "
                  << response.json.get<std::string>("error", "Unknown error")
                  << std::endl;
    }
}

void
ClientSession::postScore(int score)
{
    if (getAccountID().empty())
    {
        std::cerr << "No accountID set; cannot post score." << std::endl;
        return;
    }

    const DBResponse response =
        dbRequestManager.postPlayerScore(getAccountID(), score);

    if (response.status == 200)
    {
        std::cout << "Score posted successfully!" << std::endl;
        // Refresh local data after score update
        fetchPlayerData();
    }
    else
    {
        std::cerr << "Error posting score (" << response.status << "): "
                  << response.json.get<std::string>("error", "Unknown error")
                  << std::endl;
    }
}

StatusCode
ClientSession::sendMessage(const std::string& receiverID, const std::string& messageContent)
{
    if (getAccountID().empty())
    {
        std::cerr << "No accountID set; cannot send message." << std::endl;
        return StatusCode::ERROR_NO_SESSION_TOKEN;
    }

    if (messageContent.empty())
    {
        std::cerr << "Cannot send empty message." << std::endl;
        return StatusCode::ERROR_INVALID_MESSAGE;
    }

    const DBResponse response =
        dbRequestManager.postMessage(getAccountID(), receiverID, messageContent);

    if (response.status == 200)
    {
        std::cout << "Message sent to " << receiverID << std::endl;

        // Add the message to our local cache for this conversation
        ChatMessage newMessage;
        newMessage.from = "Me";
        newMessage.text = messageContent;

        // Store this message in our local cache
        updateLocalMessages(receiverID, newMessage);

        return StatusCode::SUCCESS;
    }
    else
    {
        std::cerr << "Error sending message (" << response.status << "): "
                  << response.json.get<std::string>("error", "Unknown error")
                  << std::endl;
        return StatusCode::ERROR_SENDING_MESSAGE;
    }
}

void
ClientSession::sendFriendRequest(const std::string& receiverID)
{
    if (getAccountID().empty())
    {
        std::cerr << "No accountID set; cannot send friend request."
                  << std::endl;
        return;
    }

    const DBResponse response =
        dbRequestManager.sendFriendRequest(getAccountID(), receiverID);

    if (response.status == 200)
    {
        std::cout << "Friend request sent to " << receiverID << std::endl;
    }
    else
    {
        std::cerr << "Error sending friend request (" << response.status
                  << "): "
                  << response.json.get<std::string>("error", "Unknown error")
                  << std::endl;
    }
}

void
ClientSession::acceptFriendRequest(const std::string& senderID)
{
    if (getAccountID().empty())
    {
        std::cerr << "No accountID set; cannot accept friend request."
                  << std::endl;
        return;
    }

    DBResponse response =
        dbRequestManager.acceptFriendRequest(getAccountID(), senderID);

    if (response.status == 200)
    {
        std::cout << "Friend request from " << senderID << " accepted!"
                  << std::endl;
        // Optionally, update local friend list
        fetchPlayerData();
    }
    else
    {
        std::cerr << "Error accepting friend request (" << response.status
                  << "): "
                  << response.json.get<std::string>("error", "Unknown error")
                  << std::endl;
    }
}

void
ClientSession::declineFriendRequest(const std::string& senderID)
{
    if (getAccountID().empty())
    {
        std::cerr << "No accountID set; cannot decline friend request."
                  << std::endl;
        return;
    }

    DBResponse response =
        dbRequestManager.declineFriendRequest(getAccountID(), senderID);

    if (response.status == 200)
    {
        std::cout << "Friend request from " << senderID << " declined."
                  << std::endl;
        // Optionally, update local pending list
        fetchPlayerData();
    }
    else
    {
        std::cerr << "Error declining friend request (" << response.status
                  << "): "
                  << response.json.get<std::string>("error", "Unknown error")
                  << std::endl;
    }
}

void
ClientSession::removeFriend(const std::string& friendID)
{
    if (getAccountID().empty())
    {
        std::cerr << "No accountID set; cannot remove friend." << std::endl;
        return;
    }

    const DBResponse response =
        dbRequestManager.removeFriend(getAccountID(), friendID);

    if (response.status == 200)
    {
        std::cout << "Friend " << friendID << " removed :(" << std::endl;
        // Optionally, update local friend list
        fetchPlayerData();
    }
    else
    {
        std::cerr << "Error removing friend (" << response.status << "): "
                  << response.json.get<std::string>("error", "Unknown error")
                  << std::endl;
    }
}

// ================== Game operations ================== //

ClientStatus
ClientSession::getOwnStatus()
{
    // this method is used to get the status of the client
    // it will return the status of the client
    return getPlayerStatus(getUsername());
}

ClientStatus
ClientSession::getPlayerStatus(const std::string& username)
{
    ServerResponse response =
        this->gameRequestManager.getPlayerStatus(username);

    // we have to check if the response was successful
    if (response.status != StatusCode::SUCCESS)
    {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                  << std::endl;
        return ClientStatus::OFFLINE;
    }

    return static_cast<ClientStatus>(std::stoi(response.data.at("status")));
}

void
ClientSession::startSession()
{
    ServerResponse response =
        this->gameRequestManager.startSession(getUsername());

    // we have to check if the response was successful
    if (response.status == StatusCode::SUCCESS_REPLACED_SESSION ||
        response.status == StatusCode::SUCCESS)
    {
        std::cout << "Session started successfully for " << getUsername()
                  << " with token " << response.data.at("token") << std::endl;
        this->setToken(response.data.at("token"));
    }
    else
    {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                  << std::endl;
    }
};

void
ClientSession::endSession()
{
    ServerResponse response = this->gameRequestManager.endSession(getToken());

    // we have to check if the response was successful
    if (response.status != StatusCode::SUCCESS)
    {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                  << std::endl;
        return;
    }

    // we set the token of the client
    this->setToken("");
};

std::unordered_map<std::string, std::string>
ClientSession::getPublicLobbiesList()
{
    ServerResponse response = this->gameRequestManager.getPublicLobbiesList();

    // we have to check if the response was successful
    if (response.status != StatusCode::SUCCESS)
    {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                  << std::endl;
        return {};
    }

    // we return the list of lobbies
    return response.data;
}

void
ClientSession::createAndJoinLobby(GameMode gameMode, int maxPlayers,
                                  bool isPublic)
{
    ServerResponse response = this->gameRequestManager.createAndJoinLobby(
        getToken(), gameMode, maxPlayers, isPublic);

    // we have to check if the response was successful
    if (response.status != StatusCode::SUCCESS)
    {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                  << std::endl;
        return;
    }
}

void
ClientSession::joinLobby(const std::string& lobbyID)
{
    ServerResponse response =
        this->gameRequestManager.joinLobby(getToken(), lobbyID);

    // we have to check if the response was successful
    if (response.status != StatusCode::SUCCESS)
    {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                  << std::endl;
        return;
    }
}

void
ClientSession::spectateLobby(const std::string& lobbyID)
{
    ServerResponse response =
        this->gameRequestManager.spectateLobby(getToken(), lobbyID);

    // we have to check if the response was successful
    if (response.status != StatusCode::SUCCESS)
    {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                  << std::endl;
        return;
    }
}

LobbyState
ClientSession::getCurrentLobbyState()
{
    ServerResponse response =
        this->gameRequestManager.getCurrentLobbyState(getToken());

    // we have to check if the response was successful
    if (response.status != StatusCode::SUCCESS)
    {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                  << std::endl;
        return LobbyState::generateEmptyState();
    }

    LobbyState lobbyState =
        LobbyState::deserialize(response.data.at("lobbyState"));
    return lobbyState;
}

void
ClientSession::leaveLobby()
{
    ServerResponse response = this->gameRequestManager.leaveLobby(getToken());

    // we have to check if the response was successful
    if (response.status != StatusCode::SUCCESS)
    {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                  << std::endl;
        return;
    }
}

void
ClientSession::readyUp()
{
    ServerResponse response = this->gameRequestManager.readyUp(getToken());

    // we have to check if the response was successful
    if (response.status != StatusCode::SUCCESS)
    {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                  << std::endl;
        return;
    }
}

void
ClientSession::unreadyUp()
{
    ServerResponse response = this->gameRequestManager.unreadyUp(getToken());

    // we have to check if the response was successful
    if (response.status != StatusCode::SUCCESS)
    {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                  << std::endl;
        return;
    }
}

void
ClientSession::sendKeyStroke(const Action& keyStroke)
{
    KeyStrokePacket keyStrokePacket;
    keyStrokePacket.token = getToken();
    keyStrokePacket.action = keyStroke;
    ServerResponse response = this->gameRequestManager.sendKeyStroke(
        keyStrokePacket.token, keyStrokePacket);

    // we have to check if the response was successful
    if (response.status != StatusCode::SUCCESS)
    {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                  << std::endl;
        return;
    }
}

PlayerState
ClientSession::getPlayerState()
{
    ServerResponse response = this->gameRequestManager.getGameState(getToken());

    // we have to check if the response was successful
    if (response.status != StatusCode::SUCCESS)
    {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                  << std::endl;
        return PlayerState::generateEmptyState();
    }

    PlayerState playerState =
        PlayerState::deserialize(response.data.at("gamestate"));
    return playerState;
}

SpectatorState
ClientSession::getSpectatorState()
{
    ServerResponse response = this->gameRequestManager.getGameState(getToken());

    // we have to check if the response was successful
    if (response.status != StatusCode::SUCCESS)
    {
        std::cerr << "Error: " << getStatusCodeString(response.status)
                  << std::endl;
        return SpectatorState::generateEmptyState();
    }

    SpectatorState spectatorState =
        SpectatorState::deserialize(response.data.at("gamestate"));
    return spectatorState;
}
