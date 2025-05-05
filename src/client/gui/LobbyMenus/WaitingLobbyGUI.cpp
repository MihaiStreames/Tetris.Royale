
#include "WaitingLobbyGUI.hpp" 


std::string getGameModeName(GameMode mode) {
    switch (mode) {
        case GameMode::CLASSIC: return "Classic";
        case GameMode::DUEL:    return "Duel";
        case GameMode::ROYALE:  return "Royale";
        case GameMode::ENDLESS: return "Endless";
        default:                return "Unknown";
    }
}

WaitingLobby::WaitingLobby(ClientSession &session, QWidget *parent)
    : QMainWindow(parent), session(session), playerList(nullptr), spectatorList(nullptr)
{
    // Set the window to fullscreen mode
    this->showFullScreen();

    lobbyPollingTimer = new QTimer(this);
    connect(lobbyPollingTimer, &QTimer::timeout, this, &WaitingLobby::pollLobbyState);
    lobbyPollingTimer->start(100);  

    setupUi();
}

WaitingLobby::~WaitingLobby() {}

void WaitingLobby::setupUi() {
    // Load custom font for the UI
    QString fontPath = QString(TETRIS_FONTS_DIR) + "/orbitron.ttf";
    QFontDatabase::addApplicationFont(fontPath);

    // Set the background and text color
    setStyleSheet("background-color: transparent; color:rgb(202, 237, 241);");

    // Define fonts for titles and interactions
    QFont fontTitle("Orbitron", 40);
    fontTitle.setLetterSpacing(QFont::AbsoluteSpacing, 6);
    fontTitle.setBold(true);

    QFont fontInteractions("Andale Mono", 20);

    // Define button styles
    QString buttonStyle = R"(
        QPushButton {
            font-family: 'Orbitron', sans-serif;
            font-size: 20px;
            background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 88, 219, 150), stop:1 rgba(31, 203, 255, 150));
            color: white;
            border: 2px solid rgb(0, 225, 255);
            border-radius: 10px;
            padding: 10px 20px;
        }
    
        QPushButton:hover {
            background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 64, 182, 150), stop:1 rgba(0, 155, 226, 150));
        }
    )";

    // Initialize lobby state and handle errors
    lobbyState = LobbyState::generateEmptyState();
    std::string errorMessage;

    try {
        // Fetch the current lobby state
        lobbyState = session.getCurrentLobbyState();

        if (lobbyState.lobbyID.empty()) {
            errorMessage = "Warning: Received empty lobby state";
        }
    } catch (const std::exception &e) {
        errorMessage = "Error getting initial lobby state: ";
        errorMessage += e.what();
    }

    // Create the central widget and layout
    QWidget *central = new QWidget(this);
    this->setCentralWidget(central);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(10);

    // Display lobby information
    QString lobbyStr = "Lobby : " + QString::fromStdString(lobbyState.lobbyID);
    QLabel* code = createLabel(lobbyStr, mainLayout, this);
    code->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    code->setFont(fontTitle);

    QString modeStr = "Game mode : " + QString::fromStdString(getGameModeName(lobbyState.gameMode));
    QLabel* gameMode = createLabel(modeStr, mainLayout, this);
    gameMode->setAlignment(Qt::AlignTop);

    QString numberPlayerStr = "Players : (" + QString::number(lobbyState.players.size()) + "/" + QString::number(lobbyState.maxPlayers) + ")";
    QLabel* playerNbr = createLabel(numberPlayerStr, mainLayout, this);
    playerNbr->setAlignment(Qt::AlignTop);

    // Create frames for player and spectator lists
    QFrame* playerListFrame = new QFrame(this);
    playerListFrame->setStyleSheet("border: 2px solid rgb(0, 225, 255); border-radius: 5px; background-color: transparent;");
    QVBoxLayout* playerListLayout = new QVBoxLayout(playerListFrame);
    playerListLayout->setContentsMargins(0, 0, 0, 0);
    playerList = createList(playerListFrame, playerListLayout);
    playerList->setStyleSheet("background-color: transparent; color: white;");
    mainLayout->addWidget(playerListFrame);

    QFrame* spectatorListFrame = new QFrame(this);
    spectatorListFrame->setStyleSheet("border: 2px solid rgb(0, 225, 255); border-radius: 5px; background-color: transparent;");
    QVBoxLayout* spectatorListLayout = new QVBoxLayout(spectatorListFrame);
    spectatorListLayout->setContentsMargins(0, 0, 0, 0);
    spectatorList = createList(spectatorListFrame, spectatorListLayout);
    spectatorList->setStyleSheet("background-color: transparent; color: white;");
    mainLayout->addWidget(spectatorListFrame);

    // Display spectator count
    QString spectatorStr = "Spectators : " + QString::number(lobbyState.spectators.size());
    QLabel* spectatorNbr = createLabel(spectatorStr, mainLayout, this);
    spectatorNbr->setAlignment(Qt::AlignTop);

    // Populate player and spectator lists
    fillPlayerListAndSpectatorList();

    // Create buttons for ready/unready and leave lobby actions
    QHBoxLayout *hozizontalLayout = new QHBoxLayout();

    QPushButton *readyBtn = createButton("Ready/Unready", hozizontalLayout, this, &WaitingLobby::changePlayerState);
    readyBtn->setFont(fontInteractions);
    readyBtn->setStyleSheet(buttonStyle);
    QPushButton *friendListBtn = createButton("Open Friends List", hozizontalLayout, this, &WaitingLobby::onFriendListBtnClicked);
    friendListBtn->setFont(fontInteractions);
    friendListBtn->setStyleSheet(buttonStyle);
    QPushButton *leaveBtn = createButton("Leave Lobby", hozizontalLayout, this, &WaitingLobby::onLeaveBtnClicked);
    leaveBtn->setFont(fontInteractions);
    leaveBtn ->setStyleSheet(buttonStyle);

    mainLayout->addLayout(hozizontalLayout);
}

void WaitingLobby::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    
    // Draw the background image
    QString bgPath = QString(TETRIS_ASSETS_DIR) + "/tetris_main.png";
    QPixmap screenPixmap(bgPath); 

    painter.drawPixmap(this->rect(), screenPixmap);
    QMainWindow::paintEvent(event); 
}

void WaitingLobby::onLeaveBtnClicked() {
    StatusCode result = session.leaveLobby();
    if (result == StatusCode::SUCCESS) {
        // Switch to the lobby menu
        buttonSwitch.changeMenu<Lobby, WaitingLobby*>(this, session);
    } else {
        QMessageBox::critical(this, "Error", QString::fromStdString("Error leaving lobby: " + getStatusCodeString(result)));
    }
}

void WaitingLobby::changePlayerState() {
    isReady = !isReady;

    StatusCode result; 
    if (isReady) {
        result = session.readyUp();
    } else {
        result = session.unreadyUp();
    }

    if (result != StatusCode::SUCCESS) {
        QMessageBox::critical(this, "Error", QString::fromStdString("Error setting ready status: " + getStatusCodeString(result)));
        return;
    }

    try {
        // Update the lobby state
        lobbyState = session.getCurrentLobbyState();
    } catch (const std::exception &e) {
        QMessageBox::critical(this, "Error", QString::fromStdString("Failed to update lobby state: " + std::string(e.what())));
        return;
    }

    // Refresh the player and spectator lists
    fillPlayerListAndSpectatorList();
}

void WaitingLobby::fillPlayerListAndSpectatorList() {
    if (!playerList || !spectatorList) {
        QMessageBox::critical(this, "Error", "playerList or spectatorList is not initialized.");
        return;
    }

    playerList->clear();
    spectatorList->clear();

    // Populate the player list
    if (lobbyState.players.empty()) {
        playerList->addItem("No players in lobby");
    } else {
        for (const auto& [token, username] : lobbyState.players) {
            bool isReady = false;
            auto it = lobbyState.readyPlayers.find(token);
            if (it != lobbyState.readyPlayers.end()) {
                isReady = it->second;
            }

            std::string statusStr = isReady ? " [Ready]" : " [Not Ready]";
            playerList->addItem(QString::fromStdString(username + statusStr));
        }
    }

    // Populate the spectator list
    if (lobbyState.spectators.empty()) {
        spectatorList->addItem("No spectators");
    } else {
        for (const auto& [token, username] : lobbyState.spectators) {
            spectatorList->addItem(QString::fromStdString(username));
        }
    }
}

void WaitingLobby::onFriendListBtnClicked() {
    auto &friendsListManager = FriendsListManager::instance();
    if (friendsListManager.friendsListWindow) {
        return;
    }

    friendsListManager.friendsListWindow = new FriendsList(session);
    friendsListManager.friendsListWindow->setAttribute(Qt::WA_DeleteOnClose);
    friendsListManager.friendsListWindow->show();

    connect(FriendsListManager::instance().friendsListWindow, &QObject::destroyed, this, []() {
        FriendsListManager::instance().friendsListWindow = nullptr;
    });
}

void WaitingLobby::pollLobbyState() {
    try {
        lobbyState = session.getCurrentLobbyState();
        fillPlayerListAndSpectatorList();

        if (session.getOwnStatus() == ClientStatus::IN_GAME) {
            lobbyPollingTimer->stop();  
            Config config(CONFIG_FILE_NAME);
            config.load();
            GameScreen *gameScreen = new GameScreen(config, session);
            gameScreen->showMaximized();
            this->close();
        }

    } catch (const std::exception &e) {
        qDebug() << "Polling error: " << e.what();
    }
}
