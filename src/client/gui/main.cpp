#include "LoginScreen.hpp"
#include <QApplication>

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    LoginScreen l;
    l.showMaximized();
    return a.exec();
}