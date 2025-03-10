#include "DBRequestManager.hpp"

namespace http = boost::beast::http;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

DBRequestManager::DBRequestManager(const std::string& host, const int& port)
    : host_(host), port_(port)
{
}

DBRequestManager::~DBRequestManager() = default;

std::string
DBRequestManager::toJSON(const boost::property_tree::ptree& pt)
{
    std::ostringstream oss;
    write_json(oss, pt, false);
    return oss.str();
}

DBResponse
DBRequestManager::sendRequest(http::verb method, const std::string& target,
                              const std::string& body)
{
    DBResponse dbResp;

    try
    {
        asio::io_context ioc;
        tcp::resolver resolver(ioc);
        auto const results = resolver.resolve(host_, std::to_string(port_));
        tcp::socket socket(ioc);
        asio::connect(socket, results.begin(), results.end());

        boost::beast::flat_buffer buffer;
        http::request<http::string_body> req{method, target, 11};
        req.set(http::field::host, host_);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        if (method == http::verb::post)
        {
            req.set(http::field::content_type, "application/json");
            req.body() = body;
            req.prepare_payload();
        }

        http::write(socket, req);

        http::response<http::string_body> res;
        http::read(socket, buffer, res);

        // Set status code in the response structure
        dbResp.status = res.result_int();

        // Parse the response body into JSON
        std::istringstream iss(res.body());
        read_json(iss, dbResp.json);

        socket.shutdown(tcp::socket::shutdown_both);
    }
    catch (const std::exception& e)
    {
        dbResp.status = 500;
        dbResp.json.put("error", e.what());
    }
    return dbResp;
}

DBResponse
DBRequestManager::getPlayer(const std::string& accountID)
{
    return sendRequest(http::verb::get, "/get_player?accountID=" + accountID,
                       "");
}

DBResponse
DBRequestManager::getLeaderboard(const int limit)
{
    return sendRequest(http::verb::get,
                       "/get_leaderboard?limit=" + std::to_string(limit), "");
}

DBResponse
DBRequestManager::getMessages(const std::string& accountID,
                              const std::string& otherAccountID)
{
    return sendRequest(http::verb::get,
                       "/get_messages?accountID=" + accountID +
                           "&otherAccountID=" + otherAccountID,
                       "");
}

DBResponse
DBRequestManager::registerPlayer(const std::string& userName,
                                 const std::string& password)
{
    boost::property_tree::ptree pt;
    pt.put("userName", userName);
    pt.put("password", password);
    return sendRequest(http::verb::post, "/register", toJSON(pt));
}

DBResponse
DBRequestManager::loginPlayer(const std::string& userName,
                              const std::string& password)
{
    boost::property_tree::ptree pt;
    pt.put("userName", userName);
    pt.put("password", password);
    return sendRequest(http::verb::post, "/login", toJSON(pt));
}

DBResponse
DBRequestManager::updatePlayer(const std::string& accountID,
                               const std::string& newName,
                               const std::string& newPassword)
{
    boost::property_tree::ptree pt;
    pt.put("accountID", accountID);
    if (!newName.empty())
    {
        pt.put("newName", newName);
    }
    if (!newPassword.empty())
    {
        pt.put("newPassword", newPassword);
    }
    return sendRequest(http::verb::post, "/update", toJSON(pt));
}

DBResponse
DBRequestManager::postPlayerScore(const std::string& accountID, const int score)
{
    boost::property_tree::ptree pt;
    pt.put("accountID", accountID);
    pt.put("score", score);
    return sendRequest(http::verb::post, "/post_score", toJSON(pt));
}

DBResponse
DBRequestManager::sendFriendRequest(const std::string& senderID,
                                    const std::string& receiverID)
{
    boost::property_tree::ptree pt;
    pt.put("accountID", senderID);
    pt.put("otherAccountID", receiverID);
    return sendRequest(http::verb::post, "/send_friend_request", toJSON(pt));
}

DBResponse
DBRequestManager::acceptFriendRequest(const std::string& receiverID,
                                      const std::string& senderID)
{
    boost::property_tree::ptree pt;
    pt.put("accountID", receiverID);
    pt.put("otherAccountID", senderID);
    return sendRequest(http::verb::post, "/accept_friend_request", toJSON(pt));
}

DBResponse
DBRequestManager::declineFriendRequest(const std::string& receiverID,
                                       const std::string& senderID)
{
    boost::property_tree::ptree pt;
    pt.put("accountID", receiverID);
    pt.put("otherAccountID", senderID);
    return sendRequest(http::verb::post, "/decline_friend_request", toJSON(pt));
}

DBResponse
DBRequestManager::removeFriend(const std::string& accountID,
                               const std::string& removedID)
{
    boost::property_tree::ptree pt;
    pt.put("accountID", accountID);
    pt.put("otherAccountID", removedID);
    return sendRequest(http::verb::post, "/remove_friend", toJSON(pt));
}

DBResponse
DBRequestManager::postMessage(const std::string& senderID,
                              const std::string& receiverID,
                              const std::string& messageContent)
{
    boost::property_tree::ptree pt;
    pt.put("accountID", senderID);
    pt.put("otherAccountID", receiverID);
    pt.put("messageContent", messageContent);
    return sendRequest(http::verb::post, "/post_message", toJSON(pt));
}

DBResponse
DBRequestManager::deleteMessage(const std::string& messageID)
{
    boost::property_tree::ptree pt;
    pt.put("messageID", messageID);
    return sendRequest(http::verb::post, "/delete_message", toJSON(pt));
}
