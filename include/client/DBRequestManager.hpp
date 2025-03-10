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

struct DBResponse
{
    boost::property_tree::ptree json;
    unsigned int status; // HTTP status code
};

class DBRequestManager
{
  public:
    DBRequestManager(const std::string& host, const int& port);

    ~DBRequestManager();

    // GET Requests
    DBResponse getPlayer(const std::string& accountID);

    DBResponse getLeaderboard(int limit = 10);

    DBResponse getMessages(const std::string& accountID,
                           const std::string& otherAccountID);

    // POST Requests
    DBResponse registerPlayer(const std::string& userName,
                              const std::string& password);

    DBResponse loginPlayer(const std::string& userName,
                           const std::string& password);

    DBResponse updatePlayer(const std::string& accountID,
                            const std::string& newName = "",
                            const std::string& newPassword = "");

    DBResponse postPlayerScore(const std::string& accountID, int score);

    DBResponse sendFriendRequest(const std::string& senderID,
                                 const std::string& receiverID);

    DBResponse acceptFriendRequest(const std::string& receiverID,
                                   const std::string& senderID);

    DBResponse declineFriendRequest(const std::string& receiverID,
                                    const std::string& senderID);

    DBResponse removeFriend(const std::string& accountID,
                            const std::string& removedID);

    DBResponse postMessage(const std::string& senderID,
                           const std::string& receiverID,
                           const std::string& messageContent);

    DBResponse deleteMessage(const std::string& messageID);

  private:
    std::string host_;
    int port_;

    static std::string toJSON(const boost::property_tree::ptree& pt);

    DBResponse sendRequest(boost::beast::http::verb method,
                           const std::string& target, const std::string& body);
};

#endif // DB_REQUEST_MANAGER_HPP
