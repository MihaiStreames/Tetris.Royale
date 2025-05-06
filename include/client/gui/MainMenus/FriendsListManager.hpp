#ifndef FRIENDSLISTMANAGER_HPP
#define FRIENDSLISTMANAGER_HPP

#include "FriendsListWidget.hpp"  
#include <QObject>

class FriendsListManager {
public:
    static FriendsListManager& instance() {
        static FriendsListManager instance;
        return instance;
    }

    FriendsList* friendsListWindow = nullptr;

    FriendsListManager(const FriendsListManager&) = delete;
    void operator=(const FriendsListManager&) = delete;

private:
    FriendsListManager() = default;
    ~FriendsListManager() = default;
};

#endif // FRIENDSLISTMANAGER_HPP
