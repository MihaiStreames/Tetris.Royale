
#include "MainMenuGUI.hpp"


MainMenu::MainMenu(ClientSession &session, QWidget *parent) : QWidget(parent), session(session) {
    QString fontPath = QString(TETRIS_FONTS_DIR) + "/orbitron.ttf";
    QFontDatabase::addApplicationFont(fontPath);
    setStyleSheet("background-color: transparent; color:rgb(202, 237, 241);");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSpacing(50);

    //----------------TITRE-------------------//
    QLabel *title = new QLabel("TETRIS  ROYALE", this);
    QFont fontTitle("Orbitron", 80);
    fontTitle.setLetterSpacing(QFont::AbsoluteSpacing, 6);
    fontTitle.setBold(true);
    title->setFont(fontTitle);
    title->setAlignment(Qt::AlignHCenter);
    title->setStyleSheet("border: 2px solid rgb(0, 225, 255); border-radius: 7px; padding: 7px;");
    mainLayout->addWidget(title);

    //----------------BOUTONS------------------//

    playButton = new QPushButton("Play", this);
    settingsButton = new QPushButton("Settings", this);
    logoutButton = new QPushButton("Logout", this);
    leaderboardButton = new QPushButton("Leaderboard", this);
    showFriendsListButton = new QPushButton("Open Friends List", this);
    

    playButton->setStyleSheet(buttonStyle);
    settingsButton->setStyleSheet(buttonStyle);
    leaderboardButton->setStyleSheet(buttonStyle);
    logoutButton->setStyleSheet(buttonStyle);
    showFriendsListButton->setStyleSheet(buttonStyle);


    mainLayout->addWidget(playButton);
    mainLayout->addWidget(settingsButton);
    mainLayout->addWidget(logoutButton);
    mainLayout->addWidget(leaderboardButton);
    mainLayout->addWidget(showFriendsListButton);

    connect(logoutButton, &QPushButton::clicked, this, &MainMenu::logout);
    connect(showFriendsListButton, &QPushButton::clicked, this, &MainMenu::showFriendsList);
    connect(playButton, &QPushButton::clicked, this, &MainMenu::openModeSelection);
    connect(settingsButton, &QPushButton::clicked, this, &MainMenu::openSettings);
    connect(leaderboardButton, &QPushButton::clicked, this, &MainMenu::openLeaderboard);


    setLayout(mainLayout);
    setWindowTitle("Main Menu");
}

MainMenu::~MainMenu() {}

void MainMenu::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QString bgPath = QString(TETRIS_ASSETS_DIR) + "/tetris_main.png";
    QPixmap screenPixmap(bgPath);

    painter.drawPixmap(this->rect(), screenPixmap);

    QWidget::paintEvent(event);
}

void MainMenu::showFriendsList() {
    FriendsList *friendsList = new FriendsList(session);
    friendsList->setAttribute(Qt::WA_DeleteOnClose);  // Supprime la fenêtre à la fermeture
    friendsList->show();
}

void MainMenu::openModeSelection() {
    ModeSelection *modeSelection = new ModeSelection(session, this);
    modeSelection->setAttribute(Qt::WA_DeleteOnClose);
    modeSelection->showMaximized(); 
    this->hide();
}

void MainMenu::openSettings() {
    SettingsScreen *settingsScreen = new SettingsScreen(this);
    settingsScreen->setAttribute(Qt::WA_DeleteOnClose);
    settingsScreen->showMaximized(); 
    this->hide();
}

void MainMenu::openLeaderboard(){
    LeaderScreen *leaderscreen = new LeaderScreen(this,session);
    leaderscreen->setAttribute(Qt::WA_DeleteOnClose);
    leaderscreen->showMaximized();
    this->hide();
}

void MainMenu::logout() {
    if(session.endSession() == StatusCode::SUCCESS){
        // Ouvrir ici le menu LOGIN
        LoginScreen *loginScreen = new LoginScreen(session);
        loginScreen->showMaximized();

        this->close();
    }
}

