#ifndef LOBBY_HPP
#define LOBBY_HPP

#include <QMainWindow>
#include <QListWidget>
#include <QSpinBox>

#include <QMessageBox>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
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

#include "menuSwitcher.hpp"
#include "ClientSession.hpp"
#include "waitingLobby.hpp"
#include "label.hpp"
#include "button.hpp"
#include "list.hpp"
#include "../MainMenus/mainMenu.hpp"

class Lobby : public QMainWindow
{
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
#endif // LOBBY_H
