#include "Client.hpp"

// This is the Client entry point
// It reads the configuration file, creates a ClientSession object and runs it

int main(int argc, char *argv[]) {

    // Ignore command line arguments for the moment
    (void) argc;
    (void) argv;

    // Load the configuration file
    Config config(CONFIG_FILE_NAME);
    config.load();

    // Create a ClientSession object and run it
    ClientSession session = ClientSession(config);
    
    // run the gui
    runTetrisClient(session);
    return EXIT_SUCCESS;

}

