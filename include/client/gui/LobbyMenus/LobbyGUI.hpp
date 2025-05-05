#ifndef LOBBYGUI_HPP
#define LOBBYGUI_HPP

#include <QMainWindow>
#include <QListWidget>
#include <QMessageBox>
#include <QTimer>
#include <QCheckBox>
#include <QSpinBox>
#include <QButtonGroup>
#include <QtWidgets>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFont>
#include <QString>
#include <QPainter>
#include <QFontDatabase>
#include <QPixmap>
#include <QWidget>

#include "LabelWidget.hpp"
#include "ButtonWidget.hpp"
#include "ListWidget.hpp"
#include "ClientSession.hpp"
#include "MenuSwitcher.hpp"
#include "WaitingLobbyGUI.hpp"
#include "MainMenuGUI.hpp"
#include "FriendsListWidget.hpp"
#include "FriendsListManager.hpp"


class Lobby : public QMainWindow {
    
    Q_OBJECT

public:
    Lobby(ClientSession& session, QWidget *parent = nullptr);
    ~Lobby();
    void setupUi();

private slots:
    void onJoinBtnClicked();
    void onSpectateClicked();
    void onBackBtnCliked();
    void onListLobbyItemSelectionChanged();
    void onRefreshBtnCliked();
    void onJoinByCodeClicked();
    void onSpectateByCodeCliked();
    void onCreateLobbyClicked(); 
    void onModeChanged();
    void onFriendListBtnClicked();
    
private:

    MenuSwitcher buttonSwitch;
    QPushButton* joinBtn;
    QPushButton* spectateBtn;
    QListWidget* listLobbies;
    QPushButton* duelBtn;
    QPushButton* classicBtn;
    QPushButton* royalBtn;
    QSpinBox* maxPlayerBox;
    ClientSession session;
    QLineEdit* codeInput;
    QCheckBox* publicCheck;

    void paintEvent(QPaintEvent *event) override;

};


#endif // LOBBYGUI_HPP
