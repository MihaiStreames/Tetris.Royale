#ifndef LEADERSCREENGUI_HPP
#define LEADERSCREENGUI_HPP

#include <QtWidgets>
#include <QLabel>
#include <QVBoxLayout>
#include <QFont>
#include <QWidget>
#include <QPainter>
#include <QFontDatabase>
#include <QTableWidget>
#include <QTableWidgetItem>

#include "MainMenuGUI.hpp"
#include "Common.hpp"
#include "ClientSession.hpp"


class LeaderScreen : public QWidget {
    Q_OBJECT
public:
    explicit LeaderScreen(MainMenu *mainMenu,ClientSession &session, QWidget *parent = nullptr);

    QString buttonStyle = R"(
        QPushButton {
            font-family: 'Orbitron', sans-serif;
            font-size: 26px;
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
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    QPushButton *backToMainButton;
    QPushButton *openFriendListButton;
    MainMenu *mainMenu;

    ClientSession &session;

private slots:
    void backToMainMenu();
    void openFriendList();
};
    
#endif
    