#pragma once

#include <atomic>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>
#include <string>
#include <utility>

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class TetrisHTTPServer
{
  public:
    TetrisHTTPServer(std::string address, unsigned short port);

    virtual ~TetrisHTTPServer();

    virtual void run();

    virtual void stop();

  protected:
    virtual void handleRequest(http::request<http::string_body> req,
                               http::response<http::string_body>& res);

    virtual std::string forwardRequest(const std::string& host,
                                       const std::string& port,
                                       const std::string& target,
                                       http::verb method,
                                       const std::string& body);

    virtual void doSession(tcp::socket socket);

  private:
    void doAccept();

    std::string address_;
    unsigned short port_;

    asio::io_context ioc_;
    tcp::acceptor acceptor_;
    std::atomic<bool> running_{false};
};
