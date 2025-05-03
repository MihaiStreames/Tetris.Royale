#ifndef MAINMENUGUI_HPP
#define MAINMENUGUI_HPP

#include <QWidget>
#include <QPushButton>
#include <QPainter>
#include <QPixmap>
#include <QVBoxLayout>
#include <QLabel>
#include <QFontDatabase>
#include <QLineEdit>
#include <QMessageBox>

#include "Common.hpp"
#include "ClientSession.hpp"

#include "LoginScreenGUI.hpp"
#include "ModeSelectionGUI.hpp"
#include "FriendsListWidget.hpp"
#include "SettingsMenuGUI.hpp"
#include "LeaderScreenGUI.hpp"

class MainMenu : public QWidget {
    Q_OBJECT
public:
    explicit MainMenu(ClientSession &session, QWidget *parent = nullptr);
    ~MainMenu();

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
    QPushButton *playButton;
    QPushButton *settingsButton;
    QPushButton *logoutButton;
    QPushButton *leaderboardButton;
    QPushButton *showFriendsListButton;

    QWidget     *chatWidget;
    QLabel      *chatDisplay;
    QLineEdit   *chatInput;

    ClientSession &session;


private slots:
    void logout();
    void showFriendsList();
    void openModeSelection();
    void openSettings();
    void openLeaderboard();

};

#endif
