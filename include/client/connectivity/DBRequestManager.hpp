#ifndef DB_REQUEST_MANAGER_HPP
#define DB_REQUEST_MANAGER_HPP

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <sstream>
#include <string>

struct DBResponse {
    boost::property_tree::ptree json;
    unsigned int status; // HTTP status code
};

class DBRequestManager {
public:
    void printResponseDebug(const DBResponse &response, const std::string &operation);

    DBResponse getAccountIDByUsername(const std::string& username) const;
    DBResponse getUsernameByAccountID(const std::string& accountID) const;

    DBRequestManager(const std::string &host, const int &port);

    ~DBRequestManager();

    [[nodiscard]] std::string getServerIP();

    [[nodiscard]] int getPort() const;

    // GET Requests
    DBResponse getPlayer(const std::string &accountID) const;

    DBResponse getLeaderboard(int limit = 10) const;

    DBResponse getMessages(const std::string &accountID,
                           const std::string &otherAccountID) const;

    // POST Requests
    DBResponse registerPlayer(const std::string &userName,
                              const std::string &password) const;

    DBResponse loginPlayer(const std::string &userName,
                           const std::string &password) const;

    DBResponse updatePlayer(const std::string &accountID,
                            const std::string &newName = "",
                            const std::string &newPassword = "") const;

    DBResponse postPlayerScore(const std::string &accountID, int score) const;

    DBResponse sendFriendRequest(const std::string &senderID,
                                 const std::string &receiverID) const;

    DBResponse acceptFriendRequest(const std::string &receiverID,
                                   const std::string &senderID) const;

    DBResponse declineFriendRequest(const std::string &receiverID,
                                    const std::string &senderID) const;

    DBResponse removeFriend(const std::string &accountID,
                            const std::string &removedID) const;

    DBResponse postMessage(const std::string &senderID,
                           const std::string &receiverID,
                           const std::string &messageContent) const;

    DBResponse deleteMessage(const std::string &messageID) const;

private:
    std::string host_;
    int port_;

    static std::string toJSON(const boost::property_tree::ptree &pt);

    DBResponse sendRequest(boost::beast::http::verb method,
                           const std::string &target, const std::string &body) const;
};

#endif // DB_REQUEST_MANAGER_HPP
