#include "LoginScreen.hpp"
#include "LeaderScreen.hpp"
#include <QApplication>

int main(int argc, char *argv[]){
    QApplication a(argc, argv);

    Config config(CONFIG_FILE_NAME);
    config.load();

    // load the session
    ClientSession session(config);

    LoginScreen l(session);

    l.showMaximized();
    return a.exec();
}