#include "friendsList.hpp"


FriendsList::FriendsList(ClientSession &session,QWidget *parent) : QMainWindow(parent),session(session)
{
    setWindowTitle("Friends List");
    setFixedSize(windowWidth, windowHeight);

    QFontDatabase::addApplicationFont("/resources/orbitron.ttf");
    setStyleSheet(windowStyle);

    // window widget 
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);


    mainLayout = new QHBoxLayout(centralWidget);

    leftSectionLayout = new QVBoxLayout();
    mainLayout->addLayout(leftSectionLayout, 2); 


    //-----FRIENDS LIST SECTION-----//
    friendsSection = new QWidget(this);
    friendsSection->setStyleSheet("border: 2px solid white; background-color: transparent;");
    leftSectionLayout->addWidget(friendsSection, 3);

    friendsSectionLayout = new QVBoxLayout(friendsSection);
    addTitle("Friends List", friendsSectionLayout, friendsSection);

    // Créer une zone scrollable
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true); 
    scrollArea->setStyleSheet("border: none;");

    friendsList = new QWidget(this);
    friendsList->setStyleSheet("border: 2px solid white; background-color: transparent;");

    friendsListLayout = new QVBoxLayout(friendsList);
    scrollArea->setWidget(friendsList);
    friendsSectionLayout->addWidget(scrollArea);

    // Ajouter la barre de recherche et le layout inférieur
    createSearchBar();
    createBottomLayout();

    //-----LOBBY INVITES SECTION-----//
    lobbySection = new QWidget(this);
    lobbySection->setStyleSheet("border: 2px solid white; background-color: transparent;");
    leftSectionLayout->addWidget(lobbySection, 1); 

    lobbySectionLayout = new QVBoxLayout(lobbySection);
    addTitle("Lobby Invites", lobbySectionLayout, lobbySection);

    lobbyInvitesList = new QWidget(this);
    lobbyInvitesList->setStyleSheet("border: 2px solid white; background-color: transparent;");
    lobbyInvitesLayout = new QVBoxLayout(lobbyInvitesList);
    lobbySectionLayout->addWidget(lobbyInvitesList);

    // Conteneur pour le chat (initialement masqué)
    chatWidget = new QWidget(this);
    chatWidget->setStyleSheet("background-color: rgb(30, 30, 30); color: white;");
    chatWidget->setFixedWidth(300); // Largeur du chat
    chatWidget->hide(); // Masquer le chat au départ
    mainLayout->addWidget(chatWidget, 1); // Le chat occupe 1/3 de l'espace
    
    (void) session.fetchPlayerData();
    populateFriends();
    checkInvites();    
    // Refresh la page
    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, [this, &session]() {
        QLayoutItem *item;
        while ((item = friendsListLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        (void) session.fetchPlayerData();
        populateFriends();
        createBottomLayout();
        checkInvites();
    });
    refreshTimer->start(5000); 
}

void FriendsList::createSearchBar() {
    // Conteneur pour la barre de recherche
    QWidget *searchBarWidget = new QWidget(this);
    searchBarWidget->setStyleSheet("border: none; background-color: transparent;");
    searchBarWidget->setFixedHeight(50);

    // Layout horizontal pour la barre de recherche
    QHBoxLayout *searchBarLayout = new QHBoxLayout(searchBarWidget);
    searchBarLayout->setContentsMargins(0, 0, 0, 0);
    searchBarLayout->setSpacing(10);

    // Champ de recherche
    QLineEdit *searchInput = new QLineEdit(searchBarWidget);
    searchInput->setPlaceholderText("Add a friend...");
    searchInput->setStyleSheet(searchBarStyle);
    searchBarLayout->addWidget(searchInput);

    // Bouton de recherche
    QPushButton *searchButton = new QPushButton("Search", searchBarWidget);
    searchButton->setStyleSheet(buttonStyle);
    searchBarLayout->addWidget(searchButton);

    // Connecter le bouton de recherche
    connect(searchButton, &QPushButton::clicked, this, [this, searchInput]() {
        QString searchText = searchInput->text().trimmed();
        if(!searchText.isEmpty()){
            (void) session.sendFriendRequest(searchText.toStdString());
            searchInput->clear();
            //Refresh the data
            (void) session.fetchPlayerData();
     
        }
    });

    // Ajouter la barre de recherche au layout cible
    friendsSectionLayout->addWidget(searchBarWidget);
}

void FriendsList::showChat(const QString &friendName) {
    // Nettoyer le layout actuel du chatWidget
    QLayout *existingLayout = chatWidget->layout();
    if (existingLayout) {
        QLayoutItem *item;
        while ((item = existingLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete existingLayout;
    }

    std::string friendID = session.getAccountIDFromUsername(friendName.toStdString());

    // Créer un nouveau layout
    QVBoxLayout *chatLayout = new QVBoxLayout(chatWidget);

    QLabel *chatTitle = new QLabel("Chatting with " + friendName, chatWidget);
    chatTitle->setStyleSheet(titleStyle);
    chatLayout->addWidget(chatTitle);

    QListWidget *messageList = new QListWidget(chatWidget);
    messageList->setStyleSheet(messageStyle);
    chatLayout->addWidget(messageList);

    QHBoxLayout *inputLayout = new QHBoxLayout();
    QLineEdit *messageInput = new QLineEdit(chatWidget);
    messageInput->setPlaceholderText("Write a message ...");
    messageInput->setStyleSheet(searchBarStyle);

    QPushButton *sendButton = new QPushButton("Send", chatWidget);
    sendButton->setStyleSheet(buttonStyle);

    inputLayout->addWidget(messageInput);
    inputLayout->addWidget(sendButton);
    chatLayout->addLayout(inputLayout);

    connect(sendButton, &QPushButton::clicked, this, [this, messageInput, friendID]() {
        QString message = messageInput->text();
        if (!message.isEmpty()) {
            (void) session.sendMessage(friendID, message.toStdString());
            messageInput->clear();
        }
    });

    // Créer un QPointer pour éviter les pointeurs morts
    QPointer<QListWidget> safeMessageList = messageList;

    messageTimer = new QTimer(this);
    connect(messageTimer, &QTimer::timeout, this, [this, friendID, safeMessageList]() {
        if (!safeMessageList) return;

        std::vector<ChatMessage> messages = session.getPlayerMessages(friendID);
        if (messages.empty()) {
            if (safeMessageList->count() == 0) {
                safeMessageList->addItem("No messages yet.");
            }
        } else {
            safeMessageList->clear();
            // Plus ancien en haut
            for (auto it = messages.rbegin(); it != messages.rend(); ++it) {
                const std::string &text = it->text;
                // ignores the invitations
                if (text.rfind("/invite ", 0) != 0) {
                    QString msgText = QString::fromStdString(it->from + ": " + it->text);
                    safeMessageList->addItem(msgText);
                }
            }
        }
    });
    messageTimer->start(100);

    chatWidget->show();
    mainLayout->addWidget(chatWidget);
}

void FriendsList::createBottomLayout(){
    if (!bottomWidget) {
        bottomWidget = new QWidget(this);
        bottomWidget->setStyleSheet("border: transparent;");
        bottomWidget->setFixedHeight(90);
        friendsSectionLayout->addWidget(bottomWidget);
        bottomLayout = new QHBoxLayout(bottomWidget);
    } else {
        // Nettoyer les anciens widgets du layout
        QLayoutItem *child;
        while ((child = bottomLayout->takeAt(0)) != nullptr) {
            if (child->widget()) {
                child->widget()->deleteLater();
            }
            delete child;
        }
    }
    QPushButton *exitButton = new QPushButton("Exit");
    exitButton->setStyleSheet(buttonStyle);

    connect(exitButton, &QPushButton::clicked, this, &QMainWindow::close);
    bottomLayout->addWidget(exitButton, 0, Qt::AlignLeft);

    // Demande d'ami la plus récente
    std::vector<std::string> &pendingRequests = session.getPendingFriendRequests();
    if(!pendingRequests.empty()){
        std::string userID = pendingRequests.back();
        pendingRequests.pop_back();
        QString friendName = QString::fromStdString(session.getRequestUsername(userID));
        FriendWidget *recentFriendRequest = new FriendWidget(friendName, FriendWidget::FriendRequest, FriendWidget::Offline, this);

        // boucle a faire
        connect(recentFriendRequest, &FriendWidget::firstButtonClicked, this, [this, userID, recentFriendRequest]() {
            (void) session.acceptFriendRequest(userID);
            (void) session.fetchPlayerData();
            createBottomLayout();
        });

        connect(recentFriendRequest, &FriendWidget::secondButtonClicked, this, [this, userID, recentFriendRequest](){
            (void) session.declineFriendRequest(userID);
            (void) session.fetchPlayerData();
            createBottomLayout();
        });

        bottomLayout->addWidget(recentFriendRequest, 1);
    }
}

void FriendsList::addFriendWidget(const QString &friendName) {
    ClientStatus status = session.getClientStatus(friendName.toStdString());
    FriendWidget::State state;

    switch (status) {
        case ClientStatus::IN_LOBBY:
            state = FriendWidget::InLobby;
            break;
        case ClientStatus::IN_GAME:
            state = FriendWidget::InGame;
            break;
        case ClientStatus::IN_MENU:
            state = FriendWidget::Online;
            break;
        case ClientStatus::OFFLINE:
            state = FriendWidget::Offline;
            break;
        default:
            state = FriendWidget::Offline; 
            break;
    } 

    QString fullName = friendName;
    switch (state) {
        case FriendWidget::Online:
            fullName += " [Online]";
            break;
        case FriendWidget::InLobby:
            fullName += " [In Lobby]";
            break;
        case FriendWidget::InGame:
            fullName += " [In Game]";
            break;
        case FriendWidget::Offline:
            fullName += " [Offline]";
            break;
        default:
            break;
    }
    
    FriendWidget *friendWidget = new FriendWidget(fullName, FriendWidget::FriendsList, state, this);

    connect(friendWidget, &FriendWidget::firstButtonClicked, this, [this, friendName]() {
        showChat(friendName);
    });

    connect(friendWidget, &FriendWidget::secondButtonClicked, this, [this, friendWidget, friendName]() {
        std::string userID = session.getAccountIDFromUsername(friendName.toStdString());
        (void) session.removeFriend(userID);
        (void) session.fetchPlayerData();
        friendsListLayout->removeWidget(friendWidget); 
        friendWidget->deleteLater(); 
    });

    //invite button
    connect(friendWidget, &FriendWidget::thirdButtonClicked, this, [this, friendName]() {
        std::string userID = session.getAccountIDFromUsername(friendName.toStdString());
        if (session.getOwnStatus() == ClientStatus::IN_LOBBY){
            sendInvite(userID);
        } else {
            QMessageBox::warning(this, "Error", "You must be in a lobby to send an invite.");
        }
    });

    //spectate button
    connect(friendWidget, &FriendWidget::fourthButtonClicked, this, [this, friendName]() {
        // if in lobby aussi bref
    });

    friendsListLayout->addWidget(friendWidget);
    friendsListLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

void FriendsList::populateFriends() {
    QLayoutItem *item;
    while ((item = friendsListLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    std::vector<std::string> &Friendslist = session.getFriendList();

    for (const auto &id : Friendslist) {
        addFriendWidget(QString::fromStdString(session.getFriendUsername(id)));
    }
}

void FriendsList::addTitle(const QString &title, QVBoxLayout *targetLayout, QWidget *targetWidget) {
    QLabel *boxTitle = new QLabel(title, targetWidget);
    boxTitle->setStyleSheet(titleStyle);
    boxTitle->setFixedHeight(30); 
    boxTitle->setAlignment(Qt::AlignLeft | Qt::AlignTop); 

    targetLayout->addWidget(boxTitle);
}

void FriendsList::clearBottomLayout() {
    if (bottomWidget) {
        friendsSectionLayout->removeWidget(bottomWidget);
        delete bottomWidget;
        bottomWidget = nullptr;
    }
}

void FriendsList::sendInvite(const std::string &friendID) {
    //hardcoded for now
    std::string lobbyID = "1234";
    std::string message = "/invite " + lobbyID;
    (void) session.sendMessage(friendID, message);
}

void FriendsList::checkInvites() {
    std::vector<std::string> &friends = session.getFriendList();

    for (const std::string &friendID : friends) {
        std::vector<ChatMessage> messages = session.getPlayerMessages(friendID);
        for (const ChatMessage &msg : messages) {
            const std::string &text = msg.text;
            if (text.rfind("/invite ", 0) == 0) {
                std::string invitedLobbyID = text.substr(8);
                if (!alreadyDisplayedInvite(friendID, invitedLobbyID)) {
                    displayLobbyInvite(friendID, invitedLobbyID);
                    markInviteAsDisplayed(friendID, invitedLobbyID);
                }
            }
        }
    }
}

void FriendsList::displayLobbyInvite(const std::string &fromID, const std::string &lobbyID) {
    QString friendName = QString::fromStdString(session.getFriendUsername(fromID));

    FriendWidget *inviteWidget = new FriendWidget(friendName, FriendWidget::LobbyInvites, FriendWidget::Offline, this);
    //Accept invitation
    connect(inviteWidget, &FriendWidget::firstButtonClicked, this, [this, inviteWidget, lobbyID]() {
        // (void) session.joinLobby(lobbyID); Trouver comment switch menu principal 
        lobbyInvitesLayout->removeWidget(inviteWidget);
        inviteWidget->deleteLater(); 
    });
    //Decline invitation
    connect(inviteWidget, &FriendWidget::secondButtonClicked, this, [this, inviteWidget]() {
        lobbyInvitesLayout->removeWidget(inviteWidget);
        inviteWidget->deleteLater(); 
    });

    lobbyInvitesLayout->addWidget(inviteWidget);
}

bool FriendsList::alreadyDisplayedInvite(const std::string &fromID, const std::string &lobbyID) {
    return displayedInvites.find({fromID, lobbyID}) != displayedInvites.end();
}

void FriendsList::markInviteAsDisplayed(const std::string &fromID, const std::string &lobbyID) {
    displayedInvites.insert({fromID, lobbyID}); 
}

void FriendsList::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    QPixmap backgroundPixmap("src/client/gui/resources/tetris_main.png");
    painter.drawPixmap(this->rect(), backgroundPixmap);

    QMainWindow::paintEvent(event);
}

