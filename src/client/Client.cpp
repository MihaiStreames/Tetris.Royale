#include "Client.hpp"


ClientSession* session = nullptr;


void signalHandler(int signum) {

    // if the signal is SIGINT, then we need to close the session
    if (signum == SIGINT) {
        std::cout << "Interrupt signal (" << signum << ") received." << std::endl;
        if (session) {
            (void) session->endSession();
        }
        exit(signum);
    }

}

// This is the Client entry point
// It reads the configuration file, creates a ClientSession object and runs it

int main(int argc, char *argv[]) {

    // configure signal handler
    signal(SIGINT, signalHandler);

    // ignore command line arguments for the moment
    (void) argc;
    (void) argv;

    // load the configuration file
    Config config(CONFIG_FILE_NAME);
    config.load();

    // load the session
    session = std::make_unique<ClientSession>(config).get();
    
    // run the gui
    runTetrisClient(*session);
    return EXIT_SUCCESS;

}

