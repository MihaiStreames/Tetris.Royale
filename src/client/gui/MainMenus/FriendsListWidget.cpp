
#include "FriendsListWidget.hpp"


FriendsList::FriendsList(ClientSession &session,QWidget *parent) : QMainWindow(parent),session(session)
{
    setWindowTitle("Friends List");
    setFixedSize(windowWidth, windowHeight);

    QString fontPath = QString(TETRIS_FONTS_DIR) + "/orbitron.ttf";
    QFontDatabase::addApplicationFont(fontPath);
    setStyleSheet(windowStyle);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QHBoxLayout(centralWidget);

    leftSectionLayout = new QVBoxLayout();
    mainLayout->addLayout(leftSectionLayout, 2); 

    friendsSection = new QWidget(this);
    friendsSection->setStyleSheet("border: 2px solid white; background-color: transparent;");
    leftSectionLayout->addWidget(friendsSection, 3);

    friendsSectionLayout = new QVBoxLayout(friendsSection);
    QLabel *boxTitle = new QLabel("Friends List", friendsSection);
    boxTitle->setStyleSheet(titleStyle);
    boxTitle->setFixedHeight(30); 
    boxTitle->setAlignment(Qt::AlignLeft | Qt::AlignTop); 

    friendsSectionLayout->addWidget(boxTitle);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true); 
    scrollArea->setStyleSheet("border: none;");

    friendsList = new QWidget(this);
    friendsList->setStyleSheet("border: 2px solid white; background-color: transparent;");

    friendsListLayout = new QVBoxLayout(friendsList);
    scrollArea->setWidget(friendsList);
    friendsSectionLayout->addWidget(scrollArea);

    createSearchBar();
    createBottomLayout();

    chatWidget = new QWidget(this);
    chatWidget->setStyleSheet("background-color: rgb(30, 30, 30); color: white;");
    chatWidget->setFixedWidth(300); 
    chatWidget->hide(); 
    mainLayout->addWidget(chatWidget, 1); 
    
    (void) session.fetchPlayerData();
    populateFriends();  
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
    });
    refreshTimer->start(5000); 
}

void FriendsList::createSearchBar() {
    QWidget *searchBarWidget = new QWidget(this);
    searchBarWidget->setStyleSheet("border: none; background-color: transparent;");
    searchBarWidget->setFixedHeight(50);

    QHBoxLayout *searchBarLayout = new QHBoxLayout(searchBarWidget);
    searchBarLayout->setContentsMargins(0, 0, 0, 0);
    searchBarLayout->setSpacing(10);

    QLineEdit *searchInput = new QLineEdit(searchBarWidget);
    searchInput->setPlaceholderText("Add a friend...");
    searchInput->setStyleSheet(searchBarStyle);
    searchBarLayout->addWidget(searchInput);

    QPushButton *searchButton = new QPushButton("Search", searchBarWidget);
    searchButton->setStyleSheet(buttonStyle);
    searchBarLayout->addWidget(searchButton);

    connect(searchButton, &QPushButton::clicked, this, [this, searchInput]() {
        QString searchText = searchInput->text().trimmed();
        if(!searchText.isEmpty()){
            (void) session.sendFriendRequest(searchText.toStdString());
            searchInput->clear();
            (void) session.fetchPlayerData();
     
        }
    });

    friendsSectionLayout->addWidget(searchBarWidget);
}

void FriendsList::showChat(const QString &friendName) {
    // clear the previous chat widget
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

    // create the chat part
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

    // Safe pointer to avoid dangling pointer issues
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
            // oldest message on top
            for (auto it = messages.rbegin(); it != messages.rend(); ++it) {
                const std::string &text = it->text;
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

    //fifo queue for friend requests
    std::vector<std::string> &pendingRequests = session.getPendingFriendRequests();
    if(!pendingRequests.empty()){
        std::string userID = pendingRequests.back();
        pendingRequests.pop_back();
        QString friendName = QString::fromStdString(session.getRequestUsername(userID));
        FriendWidget *recentFriendRequest = new FriendWidget(friendName, FriendWidget::FriendRequest, FriendWidget::Offline, this);

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

    //show chat button
    connect(friendWidget, &FriendWidget::firstButtonClicked, this, [this, friendName]() {
        showChat(friendName);
    });

    //remove friend button
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
        if (session.getClientStatus(friendName.toStdString()) == ClientStatus::IN_LOBBY) {
            (void) session.sendMessage(userID, "Join my lobby: " + session.getCurrentLobbyState().lobbyID);
        } else {
            QMessageBox::warning(this, "Invite Error", "The player is not in a lobby.");
        }
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

void FriendsList::sendInvite(const std::string &friendID) {
    LobbyState state = session.getCurrentLobbyState();
    std::string lobbyID = state.lobbyID;
    (void) session.sendMessage(friendID, "Join my lobby: " + lobbyID);
}

void FriendsList::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QString bgPath = QString(TETRIS_ASSETS_DIR) + "/tetris_main.png";
    QPixmap backgroundPixmap(bgPath);
    painter.drawPixmap(this->rect(), backgroundPixmap);

    QMainWindow::paintEvent(event);
}

