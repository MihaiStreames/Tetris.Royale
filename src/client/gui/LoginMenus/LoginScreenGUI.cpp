
#include "LoginScreenGUI.hpp"


LoginScreen::LoginScreen(ClientSession &session, QWidget *parent) : QWidget(parent), session(session){

    // Create main layout
    QString fontPath = QString(TETRIS_FONTS_DIR) + "/orbitron.ttf";
    QFontDatabase::addApplicationFont(fontPath);
    setStyleSheet("background-color: transparent; color:rgb(202, 237, 241);");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(10);

    // Title
    QWidget *titleContainer = new QWidget(this);
    QVBoxLayout *titleInputLayout = new QVBoxLayout(titleContainer);
    titleInputLayout->setAlignment(Qt::AlignTop);
    titleInputLayout->setSpacing(10);

    QLabel *title = new QLabel("TETRIS ROYALE", titleContainer);
    QFont fontTitle("Orbitron", 80);
    fontTitle.setLetterSpacing(QFont::AbsoluteSpacing, 6);
    fontTitle.setBold(true);
    title->setAlignment(Qt::AlignHCenter);
    title->setFont(fontTitle);
    title->setStyleSheet("border: 2px solid rgb(0, 225, 255); border-radius: 7px; padding: 7px;");
    titleInputLayout->addWidget(title);
    
    // Username and password input boxes
    QFont fontInteractions("Andale Mono", 26);
    username = new QLineEdit(this);
    username->setPlaceholderText("Username");
    username->setFont(fontInteractions);

    password = new QLineEdit(this);
    password->setPlaceholderText("Password");
    password->setEchoMode(QLineEdit::Password);
    password->setFont(fontInteractions);

    titleInputLayout->addWidget(username);
    titleInputLayout->addWidget(password);
    
    titleContainer->setStyleSheet("border: 2px solid rgb(0,255,255); padding: 10px; border-radius: 7px;");

    mainLayout->addWidget(titleContainer);

    // Add buttons
    QHBoxLayout *buttonsLayout = new QHBoxLayout(this);
    buttonsLayout->setSpacing(10);

    loginButton = new QPushButton("Login", this);
    registerButton = new QPushButton("Register", this);
    exitButton = new QPushButton("Exit", this);

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
            background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 64, 182, 150), stop:1 rgba(0, 155, 226, 150));
        }
    )";

    loginButton->setStyleSheet(buttonStyle);
    registerButton->setStyleSheet(buttonStyle);
    exitButton->setStyleSheet(buttonStyle);

    buttonsLayout->addWidget(loginButton);
    buttonsLayout->addWidget(registerButton);
    buttonsLayout->addWidget(exitButton);

    connect(loginButton, &QPushButton::clicked, this, &LoginScreen::loginUser);
    connect(registerButton, &QPushButton::clicked, this, &LoginScreen::openRegisterScreen);
    connect(exitButton, &QPushButton::clicked, this, &LoginScreen::exitScreen);

    mainLayout->addLayout(buttonsLayout);

    setLayout(mainLayout);

    setWindowTitle("Login Screen");
}

void LoginScreen::paintEvent(QPaintEvent *event) {
    // Paints the background

    QPainter painter(this);
    QString bgPath = QString(TETRIS_ASSETS_DIR) + "/tetris_main.png";
    QPixmap screenPixmap(bgPath);

    painter.drawPixmap(this->rect(), screenPixmap);

    QWidget::paintEvent(event);
}

void LoginScreen::openRegisterScreen(){
    // Opens the register screen and closes the login screen when clicked

    RegisterScreen *registerScreen = new RegisterScreen(session);
    registerScreen->showMaximized();

    this->hide();

}

void LoginScreen::loginUser(){
    // Logs user in when "Login" button clicked

    // Get user input
    auto usernameInput = username->text().toStdString();
    auto passwordInput = password->text().toStdString();

    // Check possible errors
    if (usernameInput.empty() || passwordInput.empty()) {
        QMessageBox::warning(this, "Error", "Please enter both username and password");
        return;
    }

    // try to login and fetch the status code
    StatusCode result = session.loginPlayer(usernameInput, passwordInput);

    if (result == StatusCode::SUCCESS) {

        StatusCode sessionResult = session.startSession();

        // if login was successful, we will try to start a new session
        if (sessionResult == StatusCode::SUCCESS) {

            // if session started successfully, we will open the main menu
            MainMenu *mainMenu = new MainMenu(session);
            mainMenu->showMaximized();

            this->hide();
        
        // if starting a session failed, we will display an error message
        } else {
            QMessageBox::warning(this, "Error", "Failed to start session.");
        }

    // if login failed, we will display an error message
    } else {
        QMessageBox::warning(this, "Error", "Login failed. Please check your credentials.");
    }
}

void LoginScreen::exitScreen(){
    // Closes the app

    this->close();
}

