#include "UDPServer.hpp"
#include <iostream>
#include <utility>

TetrisUDPServer::TetrisUDPServer(std::string  address, const unsigned short port)
    : address_(std::move(address)),
      port_(port),
      ioc_(1),
      socket_(ioc_)
{
    boost::system::error_code errorCode;
    const asio::ip::udp::endpoint endpoint(asio::ip::make_address(address_), port_);

    socket_.open(endpoint.protocol(), errorCode);
    if (errorCode) {
        std::cerr << "[UDP] Error opening socket: " << errorCode.message() << std::endl;
        return;
    }
    socket_.bind(endpoint, errorCode);
    if (errorCode) {
        std::cerr << "[UDP] Error binding socket: " << errorCode.message() << std::endl;
        return;
    }
}

TetrisUDPServer::~TetrisUDPServer() {
    TetrisUDPServer::stop();
}

void TetrisUDPServer::run() {
    running_.store(true);
    std::cout << "TetrisUDPServer listening on "
              << address_ << ":" << port_ << " (UDP)" << std::endl;

    doReceive();
    ioc_.run();
}

void TetrisUDPServer::stop() {
    if (running_.exchange(false)) {
        std::cout << "[UDP] Stopping server on " << address_ << ":" << port_ << std::endl;
        boost::system::error_code ec;
        socket_.close(ec);
        ioc_.stop();
    }
}

void TetrisUDPServer::doReceive() {
    if (!running_.load()) return;

    socket_.async_receive_from(
        asio::buffer(buffer_),
        remoteEndpoint_,
        [this](const boost::system::error_code &errorCode, const std::size_t bytesReceived) {
            if (!errorCode && bytesReceived > 0) {
                const std::string data(buffer_.data(), bytesReceived);

                // Let derived class handle
                handleUDPPacket(data, remoteEndpoint_);
            }
            // Continue receiving
            doReceive();
        }
    );
}

void TetrisUDPServer::handleUDPPacket(
    const std::string& data,
    const asio::ip::udp::endpoint& senderEndpoint)
{
    // Override in derived classes
    (void)data;
    (void)senderEndpoint;
}