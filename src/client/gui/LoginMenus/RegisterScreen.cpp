
#include "RegisterScreen.hpp"


RegisterScreen::RegisterScreen(ClientSession &session, QWidget *parent) : QWidget(parent), session(session){

    // Create main layout
    QFontDatabase::addApplicationFont("src/client/gui/resources/orbitron.ttf");
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
    
    // Username, password and password confirmation input boxes
    QFont fontInteractions("Andale Mono", 26);
    username = new QLineEdit(this);
    username->setPlaceholderText("Username");
    username->setFont(fontInteractions);

    password = new QLineEdit(this);
    password->setPlaceholderText("Password");
    password->setEchoMode(QLineEdit::Password);
    password->setFont(fontInteractions);

    confirmedPassword = new QLineEdit(this);
    confirmedPassword->setPlaceholderText("Confirm Password");
    confirmedPassword->setEchoMode(QLineEdit::Password);
    confirmedPassword->setFont(fontInteractions);

    titleInputLayout->addWidget(username);
    titleInputLayout->addWidget(password);
    titleInputLayout->addWidget(confirmedPassword);
    
    titleContainer->setStyleSheet("border: 2px solid rgb(0,255,255); padding: 10px; border-radius: 7px;");

    mainLayout->addWidget(titleContainer);

    // Add buttons
    QHBoxLayout *buttonsLayout = new QHBoxLayout(this);
    buttonsLayout->setSpacing(10);

    registerButton = new QPushButton("Register", this);
    backToLoginButton = new QPushButton("Back to Login", this);

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

    registerButton->setStyleSheet(buttonStyle);
    backToLoginButton->setStyleSheet(buttonStyle);

    buttonsLayout->addWidget(registerButton);
    buttonsLayout->addWidget(backToLoginButton);

    connect(backToLoginButton, &QPushButton::clicked, this, &RegisterScreen::openLoginScreen);
    connect(registerButton, &QPushButton::clicked, this, &RegisterScreen::registerUser);

    mainLayout->addLayout(buttonsLayout);

    setLayout(mainLayout);

    setWindowTitle("Register Screen");
}

void RegisterScreen::paintEvent(QPaintEvent *event) {
    // Paints the background

    QPainter painter(this);
    QPixmap screenPixmap("src/client/gui/resources/tetris_main.png");

    painter.drawPixmap(this->rect(), screenPixmap);

    QWidget::paintEvent(event);
}

void RegisterScreen::openLoginScreen(){
    // Opens the login screen and closes the register screen when clicked

    LoginScreen *loginScreen = new LoginScreen(session);
    loginScreen->showMaximized();

    this->close();

}

void RegisterScreen::registerUser(){
    // Registers a user when "Register" button clicked

    // Get user input
    auto usernameInput = username->text().toStdString();
    auto passwordInput = password->text().toStdString();
    auto confirmedPasswordInput = confirmedPassword->text().toStdString();

    // Check possible errors
    if (usernameInput.empty() || passwordInput.empty() || confirmedPasswordInput.empty()) {
        QMessageBox::warning(this, "Error", "All fields are required.");
        return;
    }

    if (passwordInput != confirmedPasswordInput) {
        QMessageBox::warning(this, "Error", "Passwords do not match.");
        return;
    }

    // Try to register the player
    StatusCode result = session.registerPlayer(usernameInput, passwordInput);

    if (result == StatusCode::SUCCESS) {
        openLoginScreen();
    
    // if the username is already taken, show an error message
    } else if (result == StatusCode::ERROR_USERNAME_TAKEN) {
        QMessageBox::warning(this, "Error", "Username already taken.");
        return;

    // if there was an error, show a generic error message
    } else {
        QMessageBox::warning(this, "Error", "Registration failed. Please try again.");
        return;
    }

}

