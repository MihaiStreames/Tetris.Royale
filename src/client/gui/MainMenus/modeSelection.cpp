#include "modeSelection.hpp"

ModeSelection::ModeSelection(ClientSession &session, MainMenu* mainMenuView, QWidget *parent) : QWidget(parent), mainMenu(mainMenuView), session(session){
    QFontDatabase::addApplicationFont(":/fonts/orbitron.ttf");
    setStyleSheet("background-color: transparent; color:rgb(202, 237, 241);");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSpacing(50);

    //----------------TITRE-------------------//
    QLabel *title = new QLabel("CHOOSE A MODE", this);
    QFont fontTitle("Orbitron", 80);
    fontTitle.setLetterSpacing(QFont::AbsoluteSpacing, 6);
    fontTitle.setBold(true);
    title->setFont(fontTitle);
    title->setAlignment(Qt::AlignHCenter);
    title->setStyleSheet("border: 2px solid rgb(0, 225, 255); border-radius: 7px; padding: 7px;");
    mainLayout->addWidget(title);

    //----------------BOUTONS------------------//

    soloButton = new QPushButton("Solo", this);
    multiplayerButton = new QPushButton("Mutiplayer", this);
    backToMainButton = new QPushButton("Back to main menu", this);


    soloButton->setStyleSheet(buttonStyle);
    multiplayerButton->setStyleSheet(buttonStyle);
    backToMainButton->setStyleSheet(buttonStyle);
    

    mainLayout->addWidget(soloButton);
    mainLayout->addWidget(multiplayerButton);
    mainLayout->addWidget(backToMainButton);

    connect(backToMainButton, &QPushButton::clicked, this, &ModeSelection::backToMainMenu);
    
    connect(soloButton, &QPushButton::clicked, this, [this, &session]() {
         // create and join a solo endless lobby (not public by default)
        StatusCode result = session.createAndJoinLobby(GameMode::ENDLESS, 1, false);
        if (result == StatusCode::SUCCESS){
            Config config(CONFIG_FILE_NAME);
            config.load();
            // Forcer ready state
            session.readyUp();
            GameScreen *soloGameScreen = new GameScreen(config, session);
            soloGameScreen->showMaximized();
            this->close();
        }
        
    });
    
    connect(multiplayerButton, &QPushButton::clicked, this, [this, &session]() {
        
        Lobby *lobby = new Lobby(session);
        lobby->showMaximized();

        this->hide();
        
    });
    

}

void ModeSelection::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QPixmap screenPixmap("src/client/gui/resources/tetris_main.png"); 

    painter.drawPixmap(this->rect(), screenPixmap);

    QWidget::paintEvent(event);
}

void ModeSelection::backToMainMenu() {
    mainMenu->setVisible(true);
    mainMenu->showMaximized();
    this->close();
}




