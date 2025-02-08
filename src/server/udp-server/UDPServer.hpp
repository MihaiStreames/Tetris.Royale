#pragma once

#include <boost/asio.hpp>
#include <atomic>
#include <string>
#include <array>

namespace asio = boost::asio;

class TetrisUDPServer {
public:
    TetrisUDPServer(std::string  address, unsigned short port);

    virtual ~TetrisUDPServer();

    virtual void run();
    virtual void stop();

protected:
    virtual void handleUDPPacket(
        const std::string& data,
        const asio::ip::udp::endpoint& senderEndpoint
    );

private:
    void doReceive();

    std::string             address_;
    unsigned short          port_;

    asio::io_context        ioc_;
    asio::ip::udp::socket   socket_;
    asio::ip::udp::endpoint remoteEndpoint_;

    std::atomic<bool>       running_{ false };

    // Typical max safe UDP payload: https://stackoverflow.com/questions/1098897/what-is-the-largest-safe-udp-packet-size-on-the-internet
    static constexpr size_t MAX_UDP_LENGTH = 65507;
    std::array<char, MAX_UDP_LENGTH> buffer_{};
};