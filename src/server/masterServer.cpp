#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <thread>

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

std::string forwardRequest(
    const std::string& host,
    const std::string& port,
    const std::string& target,
    http::verb method,
    const std::string& body)
{
    try {
        asio::io_context ioc;
        tcp::resolver resolver(ioc);
        auto const results = resolver.resolve(host, port);
        tcp::socket socket(ioc);
        asio::connect(socket, results.begin(), results.end());

        int version = 11;
        http::request<http::string_body> req{ method, target, version };
        req.set(http::field::host, host);
        req.set(http::field::content_type, "application/json");
        req.body() = body;
        req.prepare_payload();
        
        http::write(socket, req);
        
        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(socket, buffer, res);
        
        socket.shutdown(tcp::socket::shutdown_both);
        return res.body();
    } catch(std::exception& e) {
        std::cerr << "Error forwarding request: " << e.what() << std::endl;
        return "";
    }
}

// Handle an incoming client connection
void handleClient(beast::tcp_stream& stream) {
    beast::error_code ec;
    beast::flat_buffer buffer;

    http::request<http::string_body> req;
    http::read(stream, buffer, req, ec);
    if (ec) {
        std::cerr << "read error: " << ec.message() << std::endl;
        return;
    }

    // Forward DB reqs to DB server
    const auto target(std::string(req.target()));
    std::string DBResponse;

    if (target == "/register" || target == "/login" ||
        target == "/update"   || target == "/add_friend" ||
        target.find("/get_player") == 0) {
        DBResponse = forwardRequest("127.0.0.1", "8081", target, req.method(), req.body());

        } else {
            DBResponse = R"({"error": "Unknown DB endpoint"})";
        }

    // Return the DB server's response back to the client
    http::response<http::string_body> res{http::status::ok, req.version()};
    res.set(http::field::content_type, "application/json");
    res.body() = DBResponse;
    res.prepare_payload();
    http::write(stream, res, ec);
    if (ec)
        std::cerr << "write error: " << ec.message() << std::endl;
}

// Manage a session for a connected client
void doSession(tcp::socket socket) {
    try {
        beast::tcp_stream stream(std::move(socket));
        handleClient(stream);
        stream.socket().shutdown(tcp::socket::shutdown_send);
    } catch(std::exception& e) {
        std::cerr << "Session error: " << e.what() << std::endl;
    }
}

int main() {
    try {
        auto const address = asio::ip::make_address("0.0.0.0");
        unsigned short port = 8080;
        asio::io_context ioc{1};
        tcp::acceptor acceptor(ioc, {address, port});
        std::cout << "Master Server listening on port " << port << std::endl;
        for(;;) {
            tcp::socket socket{ioc};
            acceptor.accept(socket);
            std::thread([sock = std::move(socket)]() mutable { doSession(std::move(sock)); }).detach();
        }
    } catch(std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}