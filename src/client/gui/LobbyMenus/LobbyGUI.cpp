
#include "LobbyGUI.hpp"


std::string getGameModeString(GameMode mode) {
    switch (mode) {
        case GameMode::CLASSIC: return "Classic";
        case GameMode::DUEL:    return "Duel";
        case GameMode::ROYALE:  return "Royale";
        case GameMode::ENDLESS: return "Endless";
        default:                return "Unknown";
    }
}

Lobby::Lobby(ClientSession& session, QWidget *parent)
    : QMainWindow(parent), session(session) {

    // Load custom font for the UI
    QString fontPath = QString(TETRIS_FONTS_DIR) + "/orbitron.ttf";
    QFontDatabase::addApplicationFont(fontPath);

    // Set the background and text color
    setStyleSheet("background-color: transparent; color:rgb(202, 237, 241);");

    // Set the window title
    setWindowTitle("Lobby");

    // Initialize the UI components
    setupUi();

}

Lobby::~Lobby() {}

void Lobby::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    // Draw the background image
    QString bgPath = QString(TETRIS_ASSETS_DIR) + "/tetris_main.png";
    QPixmap screenPixmap(bgPath);
    painter.drawPixmap(this->rect(), screenPixmap);

    QMainWindow::paintEvent(event);
}

void Lobby::setupUi() {
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

    // Create the central widget and layout
    QWidget *central = new QWidget(this);
    this->setCentralWidget(central);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(10);

    // Add a label for the lobby browser
    QLabel* lobbyBrowserLabel = createLabel("Lobby Browser", mainLayout, this);
    lobbyBrowserLabel->setAlignment(Qt::AlignHCenter);
    lobbyBrowserLabel->setFont(fontTitle);

    // Add a label for available lobbies
    QLabel* availableLobbyLabel = createLabel("Available Lobbies", mainLayout, this);
    availableLobbyLabel->setFont(fontInteractions);

    // Create a frame for the lobby list
    QFrame* lobbyListFrame = new QFrame(this);
    lobbyListFrame->setStyleSheet("border: 2px solid rgb(0, 225, 255); border-radius: 5px; background-color: transparent;");
    QVBoxLayout* lobbyListLayout = new QVBoxLayout(lobbyListFrame);
    lobbyListLayout->setContentsMargins(0, 0, 0, 0);
    listLobbies = createList(this, lobbyListLayout);
    listLobbies->setStyleSheet("background-color: transparent; color: white;");
    mainLayout->addWidget(lobbyListFrame);

    // Connect the lobby list selection change signal
    connect(listLobbies, &QListWidget::itemSelectionChanged, this, &Lobby::onListLobbyItemSelectionChanged);

    // Refresh the lobby list
    onRefreshBtnCliked();

    // listLobbies = createList(this, mainLayout);
    // listLobbies->setStyleSheet("background-color: transparent; color: white;");
    // onRefreshBtnCliked();

    QHBoxLayout *hozizontalLayout = new QHBoxLayout();

    // Add buttons for joining, spectating, and refreshing lobbies
    joinBtn = createButton("Join", hozizontalLayout, this, &Lobby::onJoinBtnClicked);
    joinBtn->setStyleSheet(buttonStyle);
    joinBtn->setEnabled(false);

    spectateBtn = createButton("Spectate", hozizontalLayout, this, &Lobby::onSpectateClicked);
    spectateBtn->setStyleSheet(buttonStyle);
    spectateBtn->setEnabled(false);

    QPushButton* refreshBtn = createButton("Refresh", hozizontalLayout, this, &Lobby::onRefreshBtnCliked);
    refreshBtn->setStyleSheet(buttonStyle);

    // Add input field for joining by code
    codeInput = new QLineEdit(this);
    codeInput->setPlaceholderText("Code");
    codeInput->setMaxLength(16);
    hozizontalLayout->addWidget(codeInput);

    QPushButton* joinByCodeBtn = createButton("Join By Code", hozizontalLayout, this, &Lobby::onJoinByCodeClicked);
    joinByCodeBtn->setStyleSheet(buttonStyle);

    QPushButton* spectateByCodeBtn = createButton("Spectate By Code", hozizontalLayout, this, &Lobby::onSpectateByCodeCliked);
    spectateByCodeBtn->setStyleSheet(buttonStyle);

    mainLayout->addLayout(hozizontalLayout);

    // Add a label for creating a lobby
    QLabel* createLobbyLabel = createLabel("Create Lobby", mainLayout, this);
    createLobbyLabel->setAlignment(Qt::AlignHCenter);
    createLobbyLabel->setFont(fontInteractions);

    // Create a button group for game modes
    QButtonGroup* modeGroup = new QButtonGroup(this);
    modeGroup->setExclusive(true);

    QHBoxLayout* modeLayout = new QHBoxLayout();

    // Add buttons for different game modes
    duelBtn = new QPushButton("Duel");
    duelBtn->setStyleSheet(buttonStyle);
    classicBtn = new QPushButton("Classic");
    classicBtn->setStyleSheet(buttonStyle);
    royalBtn = new QPushButton("Royal");
    royalBtn->setStyleSheet(buttonStyle);

    connect(classicBtn, &QPushButton::toggled, this, &Lobby::onModeChanged);
    connect(royalBtn, &QPushButton::toggled, this, &Lobby::onModeChanged);
    connect(duelBtn, &QPushButton::toggled, this, &Lobby::onModeChanged);

    duelBtn->setCheckable(true);
    classicBtn->setCheckable(true);
    royalBtn->setCheckable(true);

    modeLayout->addWidget(duelBtn);
    modeLayout->addWidget(classicBtn);
    modeLayout->addWidget(royalBtn);

    modeGroup->addButton(duelBtn, 1);
    modeGroup->addButton(classicBtn, 2);
    modeGroup->addButton(royalBtn, 3);

    mainLayout->addLayout(modeLayout);

    // Add a spin box for setting the maximum number of players
    QLabel* maxPlayerLabel = new QLabel("Max joueurs :");
    maxPlayerLabel->setFont(fontInteractions);
    maxPlayerBox = new QSpinBox(this);
    maxPlayerBox->setRange(3, 9);
    maxPlayerBox->setValue(3);
    maxPlayerBox->setEnabled(false);

    QHBoxLayout* maxPlayerLayout = new QHBoxLayout();
    maxPlayerLayout->addWidget(maxPlayerLabel);
    maxPlayerLayout->addWidget(maxPlayerBox);

    QWidget* maxPlayerWidget = new QWidget(this);
    maxPlayerWidget->setLayout(maxPlayerLayout);
    mainLayout->addWidget(maxPlayerWidget);

    // Add a checkbox for public/private lobby
    publicCheck = new QCheckBox("Public", this);
    mainLayout->addWidget(publicCheck);

    // Add buttons for creating and going back
    QPushButton* createBtn = createButton("Create", mainLayout, this, &Lobby::onCreateLobbyClicked);
    createBtn->setStyleSheet(buttonStyle);

    QPushButton *friendListBtn = createButton("Open Friends List", mainLayout, this, &Lobby::onFriendListBtnClicked);
    friendListBtn->setStyleSheet(buttonStyle);

    QPushButton* backBtn = createButton("Back", mainLayout, this, &Lobby::onBackBtnCliked);
    backBtn->setStyleSheet(buttonStyle);
}

std::pair<std::vector<std::string>, std::vector<LobbyState> >
parseLobbies(const std::unordered_map<std::string, std::string> &lobbyData) {
    std::vector<std::string> lobbyIds;
    std::vector<LobbyState> lobbyStates;
    
    for (const auto &[idx, data]: lobbyData) {
        try {
            LobbyState lobbyState = LobbyState::deserialize(data);
            lobbyIds.push_back(lobbyState.lobbyID);
            lobbyStates.push_back(lobbyState);
        } catch (const std::exception &e) {
            continue;
        }
    }
    
    return {lobbyIds, lobbyStates};
}

void Lobby::onRefreshBtnCliked() {
    listLobbies->clear(); 
    
    std::string errorMessage;
    
    auto lobbiesData = session.getPublicLobbiesList();
    auto [lobbyIds, lobbyStates] = parseLobbies(lobbiesData);
    
    std::vector<std::string> lobbyOptions;
    if (lobbyStates.empty()) {
        listLobbies->addItem("No available lobbies");
    } else {
        for (const auto &state: lobbyStates) {
            std::string gameModeName;
            
            switch (state.gameMode) {
                case GameMode::CLASSIC: gameModeName = "Classic";
                break;
                case GameMode::DUEL: gameModeName = "Duel";
                break;
                case GameMode::ROYALE: gameModeName = "Royale";
                break;
                default: gameModeName = "Unknown";
            }
            
            std::stringstream ss;
            ss << "ID: " << state.lobbyID
            << " | Mode: " << gameModeName
            << " | Players: " << state.players.size() << "/" << state.maxPlayers
            << " | Spectators: " << state.spectators.size();
            
            listLobbies->addItem(QString::fromStdString(ss.str()));
        }
    }
}

void Lobby::onFriendListBtnClicked() {
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

void Lobby::onJoinByCodeClicked() {
    QString lobbyCodeInput = codeInput->text(); 

    if (!lobbyCodeInput.isEmpty()) {
        StatusCode result = session.joinLobby(lobbyCodeInput.toStdString());
        if (result == StatusCode::SUCCESS) {
            buttonSwitch.changeMenu<WaitingLobby, Lobby*>(this, session);
        } else {
            QMessageBox::critical(this, "Error", QString::fromStdString("Failed to join lobby by code: " + getStatusCodeString(result)));
        }
    } else {
        QMessageBox::critical(this, "Error", "Please enter a valid lobby code.");
    }
}

void Lobby::onSpectateByCodeCliked() { 
    QString lobbyCodeInput = codeInput->text(); 

    if (!lobbyCodeInput.isEmpty()) {
        StatusCode result = session.spectateLobby(lobbyCodeInput.toStdString());
        if (result == StatusCode::SUCCESS) {
            buttonSwitch.changeMenu<WaitingLobby, Lobby*>(this, session);
        } else {
            QMessageBox::critical(this, "Error", QString::fromStdString("Failed to join lobby by code: " + getStatusCodeString(result)));
        }
    } else {
        QMessageBox::critical(this, "Error", "Please enter a valid lobby code.");
    }
 }

void Lobby::onCreateLobbyClicked() {
    if (session.getToken().empty()) {
        QMessageBox::critical(this, "Error", "Error: Not logged in or session token missing");
        return;
    }

    GameMode mode;
    if (classicBtn->isChecked()) {
        mode = GameMode::CLASSIC;
    } else if (duelBtn->isChecked()) {
        mode = GameMode::DUEL;
    } else if (royalBtn->isChecked()) {
        mode = GameMode::ROYALE;
    } else {
        mode = GameMode::CLASSIC; 
    }

    int maxPlayers = maxPlayerBox->value();

    bool isPublic = publicCheck->isChecked();

    if (mode == GameMode::DUEL) {
        maxPlayers = 2; 
    }

    StatusCode result = session.createAndJoinLobby(mode, maxPlayers, isPublic);

    if (result == StatusCode::SUCCESS) {
        buttonSwitch.changeMenu<WaitingLobby, Lobby*>(this, session);
    } else {
        QMessageBox::critical(this, "Error", QString::fromStdString("Failed to create lobby: " + getStatusCodeString(result)));
    }
}

void Lobby::onModeChanged() {
    bool enabled = classicBtn->isChecked() || royalBtn->isChecked();
    maxPlayerBox->setEnabled(enabled);
}

void Lobby::onSpectateClicked() {
    auto lobbiesData = session.getPublicLobbiesList();
    auto [lobbyIds, lobbyStates] = parseLobbies(lobbiesData);

    if (lobbyIds.empty() || (listLobbies->count() == 1 && listLobbies->item(0)->text() == "No available lobbies")) {
        QMessageBox::critical(this, "Error", "No lobbies available to spectate");
        return;
    }

    int selectedLobby = listLobbies->currentRow();
    if (selectedLobby < 0 || selectedLobby >= static_cast<int>(lobbyIds.size())) {
        QMessageBox::critical(this, "Error", "Invalid lobby selection");
        return;
    }

    StatusCode result = session.spectateLobby(lobbyIds[selectedLobby]);
    if (result == StatusCode::SUCCESS) {
        buttonSwitch.changeMenu<WaitingLobby, Lobby*>(this, session);
    } else {
        QMessageBox::critical(this, "Error", QString::fromStdString("Failed to spectate lobby: " + getStatusCodeString(result)));
    }
}

void Lobby::onBackBtnCliked() {
    MainMenu* mainMenu = new MainMenu(session);
    mainMenu->showMaximized();

    this->hide();
}

void Lobby::onListLobbyItemSelectionChanged() {
    if (listLobbies->selectedItems().isEmpty()) {
        joinBtn->setEnabled(false);
        spectateBtn->setEnabled(false);
    } else {
        joinBtn->setEnabled(true);
        spectateBtn->setEnabled(true);
    }
}

void Lobby::onJoinBtnClicked() {
    auto lobbiesData = session.getPublicLobbiesList();
    auto [lobbyIds, lobbyStates] = parseLobbies(lobbiesData);

    QString selectedLobbyText = listLobbies->currentItem()->text();
    int selectedLobbyID = selectedLobbyText.section("ID: ", 1, 1).section(" |", 0, 0).toInt();

    if (!lobbyIds.empty() && selectedLobbyID < static_cast<int>(lobbyIds.size())) {
        StatusCode result = session.joinLobby(lobbyIds[selectedLobbyID]);
        if (result == StatusCode::SUCCESS) {
            buttonSwitch.changeMenu<WaitingLobby, Lobby*>(this, session);
        } else {
            QMessageBox::critical(this, "Error", 
                QString::fromStdString("Failed to join lobby: " + getStatusCodeString(result)));
        }
    }
}

