#include "HTTPServer.hpp"

TetrisHTTPServer::TetrisHTTPServer(std::string address,
                                   const unsigned short port)
    : address_(std::move(address)), port_(port), ioc_(1), acceptor_(ioc_)
{
    boost::system::error_code errorCode;
    const asio::ip::tcp::endpoint endpoint(asio::ip::make_address(address_),
                                           port_);

    acceptor_.open(endpoint.protocol(), errorCode);
    if (errorCode)
    {
        std::cerr << "[HTTP] Error opening acceptor: " << errorCode.message()
                  << std::endl;
        return;
    }
    acceptor_.bind(endpoint, errorCode);
    if (errorCode)
    {
        std::cerr << "[HTTP] Error binding acceptor: " << errorCode.message()
                  << std::endl;
        return;
    }
    acceptor_.listen(asio::socket_base::max_listen_connections, errorCode);
    if (errorCode)
    {
        std::cerr << "[HTTP] Error listening on acceptor: "
                  << errorCode.message() << std::endl;
        return;
    }
}

TetrisHTTPServer::~TetrisHTTPServer() { TetrisHTTPServer::stop(); }

void
TetrisHTTPServer::run()
{
    running_.store(true);
    std::cout << "[HTTP] [INFO] TetrisHTTPServer listening on " << address_
              << ":" << port_ << " (TCP/HTTP)" << std::endl;

    doAccept();
    ioc_.run();
}

void
TetrisHTTPServer::stop()
{
    if (running_.exchange(false))
    {
        std::cout << "[HTTP] [INFO] Stopping server on " << address_ << ":"
                  << port_ << std::endl;
        boost::system::error_code ec;
        acceptor_.close(ec);
        ioc_.stop();
    }
}

void
TetrisHTTPServer::doAccept()
{
    if (!running_.load())
    {
        return; // Bail out
    }

    acceptor_.async_accept(
        [this](const boost::system::error_code& errorCode, tcp::socket socket)
        {
            if (!errorCode)
            {
                // Launch a new session in its own thread
                std::thread(&TetrisHTTPServer::doSession, this,
                            std::move(socket))
                    .detach();
            }
            // Accept the next connection
            doAccept();
        });
}

void
TetrisHTTPServer::doSession(tcp::socket socket)
{
    beast::error_code ec;
    beast::flat_buffer buffer;

    for (;;)
    {
        http::request<http::string_body> req;
        http::read(socket, buffer, req, ec);

        if (ec == http::error::end_of_stream)
        {
            break; // Remote closed
        }

        if (ec)
        {
            std::cerr << "[HTTP] [ERROR] Read error: " << ec.message()
                      << std::endl;
            break;
        }

        // Prepare a basic response
        http::response<http::string_body> res{http::status::ok, req.version()};
        res.set(http::field::content_type, "text/plain");

        // Let the derived class handle this request
        handleRequest(req, res);

        // Send the response
        http::write(socket, res, ec);
        if (ec)
        {
            std::cerr << "[HTTP] [ERROR] Write error: " << ec.message()
                      << std::endl;
            break;
        }

        if (res.need_eof())
        {
            break;
        }
    }

    socket.shutdown(tcp::socket::shutdown_send, ec);
}

void
TetrisHTTPServer::handleRequest(http::request<http::string_body> req,
                                http::response<http::string_body>& res)
{
    res.body() = "[HTTP] [INFO] Received request body:\n" + req.body();
    res.prepare_payload();
}

std::string
TetrisHTTPServer::forwardRequest(const std::string& host,
                                 const std::string& port,
                                 const std::string& target, http::verb method,
                                 const std::string& body)
{
    try
    {
        asio::io_context forwardIoc;
        tcp::resolver resolver(forwardIoc);
        auto const results = resolver.resolve(host, port);

        tcp::socket socket(forwardIoc);
        asio::connect(socket, results.begin(), results.end());

        // Build the request
        constexpr int version = 11; // HTTP/1.1
        http::request<http::string_body> req{method, target, version};
        req.set(http::field::host, host);
        req.set(http::field::content_type, "application/json");
        req.body() = body;
        req.prepare_payload();

        // Send request
        http::write(socket, req);

        // Read response
        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(socket, buffer, res);

        socket.shutdown(tcp::socket::shutdown_both);
        return res.body();
    }
    catch (std::exception& e)
    {
        std::cerr << "[HTTP] [ERROR] Error forwarding request: " << e.what()
                  << std::endl;
        return "";
    }
}