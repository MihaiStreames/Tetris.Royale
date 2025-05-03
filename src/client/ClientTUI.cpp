#include "ClientTUI.hpp"


// This is the Client TUI entry point
// It reads the configuration file, creates a ClientSession object and runs it

int main(int argc, char *argv[]) {

    // ignore command line arguments for the moment
    (void) argc;
    (void) argv;

    // load the configuration file
    Config config(CONFIG_FILE_NAME);
    config.load();

    // load the session
    ClientSession session(config);
    
    // run the gui
    runTetrisClient(session, config);
    return EXIT_SUCCESS;

}

