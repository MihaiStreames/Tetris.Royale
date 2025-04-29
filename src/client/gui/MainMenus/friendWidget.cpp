#include "friendWidget.hpp"

FriendWidget::FriendWidget(const QString &friendName, Context context,State state, QWidget *parent)
    : QWidget(parent), friendName(friendName),context(context) {
    setupLayout(context);
    updateState(state);
}

void FriendWidget::setupLayout(Context context) {
    QHBoxLayout *layout = new QHBoxLayout(this);

    // Bouton principal pour afficher le nom de l'ami
    mainButton = new QPushButton(friendName, this);
    mainButton->setStyleSheet(buttonStyle);
    layout->addWidget(mainButton);

    // Boutons d'options
    firstOptionButton = new QPushButton(this);
    firstOptionButton->setStyleSheet(buttonStyle);
    
    secondOptionButton = new QPushButton(this);
    secondOptionButton->setStyleSheet(buttonStyle);

    thirdOptionButton = new QPushButton(this);
    thirdOptionButton->setStyleSheet(buttonStyle);

    fourthOptionButton = new QPushButton(this);
    fourthOptionButton->setStyleSheet(buttonStyle);

    // Définir les textes des boutons en fonction du contexte
    switch (context) {
        case FriendsList:
            firstOptionButton->setText("Chat");
            secondOptionButton->setText("Remove");
            thirdOptionButton->setText("Invite");
            fourthOptionButton->setText("Spectate");
            break;
        case LobbyInvites:
            firstOptionButton->setText("Join");
            secondOptionButton->setText("Decline");
            break;
        case FriendRequest:
            firstOptionButton->setText("Accept");
            secondOptionButton->setText("Decline");
            break;
        default:
            break;
    }
    
    stateCircle = new QLabel(this);
    stateCircle->setFixedSize(15, 15);
    stateCircle->setStyleSheet("background-color: gray; border-radius: 6px; border-color: black");
    // State uniquement dans la friendsList
    if (context == FriendsList) {
        layout->addWidget(stateCircle);
    } else {
        stateCircle->setVisible(false);
    }
    firstOptionButton->setVisible(false);
    secondOptionButton->setVisible(false);
    thirdOptionButton->setVisible(false);
    fourthOptionButton->setVisible(false);
    
    layout->addWidget(firstOptionButton);
    layout->addWidget(secondOptionButton);
    layout->addWidget(thirdOptionButton);
    layout->addWidget(fourthOptionButton);

    connect(mainButton, &QPushButton::clicked, this, [this, context](){
        bool areOptionsVisible = firstOptionButton->isVisible();
        firstOptionButton->setVisible(!areOptionsVisible);
        secondOptionButton->setVisible(!areOptionsVisible);
        if (context == FriendsList){
            thirdOptionButton->setVisible(!areOptionsVisible);
            fourthOptionButton->setVisible(!areOptionsVisible);
        }
    });
    // Connecter les signaux des boutons 
    connect(firstOptionButton, &QPushButton::clicked, this, [this]() {
        emit firstButtonClicked(friendName);
    });
    connect(secondOptionButton, &QPushButton::clicked, this, [this]() {
        emit secondButtonClicked(friendName);
    });
    connect(thirdOptionButton, &QPushButton::clicked, this, [this]() {
        emit thirdButtonClicked(friendName);
    });
    connect(fourthOptionButton, &QPushButton::clicked, this, [this]() {
        emit fourthButtonClicked(friendName);
    });
}

void FriendWidget::updateState(State state) {
    QString color;
    switch (state) {
        case Online:  
            color = "green"; 
            break;
        case InLobby:
            color = "orange";
            break;
        case InGame:  
            color = "red"; 
            break;
        case Offline: 
            color = "gray"; 
            break;
        default:
            color = "gray"; 
            break;
    }
    
    stateCircle->setStyleSheet(QString("background-color: %1; border-radius: 6px; border-color : black").arg(color));
    
}
