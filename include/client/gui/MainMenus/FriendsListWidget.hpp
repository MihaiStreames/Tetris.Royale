#ifndef FRIENDSLISTWIDGET_HPP
#define FRIENDSLISTWIDGET_HPP

#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QWidget>
#include <QMap>
#include <QDebug>
#include <QPainter>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QFrame>
#include <QListWidget>
#include <QFontDatabase>
#include <QTimer>
#include <QPointer>
#include <QMessageBox>

#include <set>
#include <sstream>
#include <random>

#include "FriendWidget.hpp"
#include "ClientSession.hpp"


class FriendsList : public QMainWindow {
    Q_OBJECT

public:
    explicit FriendsList(ClientSession &session,QWidget *parent = nullptr);

    QString buttonStyle = R"(
        QPushButton {
            font-size: 14px;
            font-family: 'Orbitron', sans-serif;
            background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 88, 219, 150), stop:1 rgba(31, 203, 255, 150));
            color: white;
            border: 2px solid rgb(0, 225, 255);
            border-radius: 10px;
            padding: 10px 20px;
        }
        QPushButton:hover {
            background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 64, 182, 150), stop:1 rgba(0, 155, 226, 150));
        }
    )";

    QString searchBarStyle = R"(
        QLineEdit {
            font-family: 'Orbitron', sans-serif;
            font-size: 14px;
            padding: 5px;
            border: 2px solid rgb(0, 225, 255);
            border-radius: 5px;
            background-color: white;
            color: black;
        }
    )";

    QString windowStyle = R"(
        QMainWindow {
            background-color: black; /* Fond noir */
            color: rgb(202, 237, 241); /* Couleur du texte */
        }
    )";

    QString titleStyle = R"(
        QLabel {
            font-size: 14px;
            font-family: 'Orbitron', sans-serif;
            font-weight: bold;
            color: rgb(202, 237, 241);
            padding: 5px;
            border: transparent;
            background-color: transparent;
        }
    )";

    QString messageStyle = R"(
        font-family: 'Orbitron', sans-serif;
        background-color: rgb(20, 20, 20);
        color: rgb(202, 237, 241);
        font-size: 14px;
        border: 2px solid rgb(0, 225, 255);
        border-radius: 5px;
        padding: 5px;
    )";

protected:
    void paintEvent(QPaintEvent *event) override; // Déclaration de paintEvent

private:
    // window dimensions
    const int windowHeight = 800;
    const int windowWidth  = 1100;


    //widgets and layouts
    QWidget *centralWidget;
    QHBoxLayout *mainLayout;
    QWidget *bottomWidget = nullptr;
    QHBoxLayout *bottomLayout = nullptr;

    
    QVBoxLayout *leftSectionLayout;
    QWidget *rightSection;
    
    // FRIENDS LIST SECTION
    QWidget *friendsSection;
    QVBoxLayout *friendsSectionLayout;
    QWidget *friendsList;
    QVBoxLayout *friendsListLayout;
    QScrollArea *scrollArea;

    QTimer *refreshTimer; 
    
    QWidget *chatWidget;
    QTimer *messageTimer;


    ClientSession &session;

private slots:
    void showChat(const QString &friendName);
    void addFriendWidget(const QString &friendName);
    void populateFriends();
    void createSearchBar();
    void createBottomLayout();
    void sendInvite(const std::string &friendID);
};

#endif // FRIENDSLIST_HPP