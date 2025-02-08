#include "master-server/masterServer.hpp"
#include <iostream>

int main() {
    // DB server is at 127.0.0.1:8081
    // Game server will be at 127.0.0.1:9001
    // Master server listens on port 8080 (HTTP) and 9090 (UDP)
    try {
        TetrisMasterServer server(
            /* httpAddress */ "0.0.0.0",
            /* httpPort    */ 8080,
            /* udpAddress */  "0.0.0.0",
            /* udpPort    */  9090,
            /* dbHost   */     "127.0.0.1",
            /* dbPort   */     "8081",
            /* gameHost */     "127.0.0.1",
            /* gamePort */     "9001"
        );

        // Start both HTTP & UDP servers
        server.run();

        std::cout << "Master Server running. Press Enter to stop...\n";
        std::cin.get();

        server.stop();
        std::cout << "Master Server stopped.\n";
    }

    catch(std::exception& e) {
        std::cerr << "Master Server exception: " << e.what() << std::endl;
    }

    return 0;
}