
#include "LoginMenus/LoginScreen.hpp"
#include <QApplication>


// This is the Client TUI entry point
// It reads the configuration file, creates a ClientSession object and runs it

int main(int argc, char *argv[]) {

    // Set the locale to the user's default locale
    QApplication a(argc, argv);

    // Load the configuration file
    Config config(CONFIG_FILE_NAME);
    config.load();

    // Load the session
    ClientSession session(config);

    // Create the main window
    LoginScreen loginScreen(session);
    loginScreen.showMaximized();

    return a.exec();

}
