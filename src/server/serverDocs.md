# Networking and Protocols Documentation

## Architecture
### **Master Server**
- Handles **authentication, matchmaking, and database interactions**.
- Forwards **database requests** to the **database server**.
- Redirects players to the **game server** when a match starts.
- Knows when a **game ends** (optional stat tracking).

### **Game Server**
- Manages **ongoing game sessions**.
- Clients send **keystrokes** to the server.
- Server validates inputs and sends **full game state updates** back.
- Spectators can view **other players' matches**.
- Sessions remain open until the game ends.

### **Database Server**
- Stores **player accounts, statistics, and matchmaking data**.
- Handles **all persistent data storage and retrieval**.
- Communicates with the **master server** for authentication and updates.

## Protocols and Communication
### **Master Server Communication**
| Component                      | Protocol                        | Purpose                                         |
|--------------------------------|---------------------------------|-------------------------------------------------|
| **Master Server**              | **TCP (HTTP)**                  | Lobby management, matchmaking, database queries |
| **Game Server (During Match)** | **UDP (keystrokes)**            | Input handling                                  |
|                                | **TCP (game state updates)**    | Full board state, scores, power-ups             |
| **Clients**                    | **TCP (persistent connection)** | Receiving game states                           |
| **Spectators**                 | **TCP only**                    | Watching matches in real time                   |

### **Client-Server Data Exchange**
#### **Client → Server (Keystrokes - UDP)**
```cpp
struct KeystrokePacket {
    UUID playerID;
    UUID gameID;
    Action action;          // Move left, right, rotate, drop, power-up
    int timestamp;
};
```
#### **Server → Client (Full Game State - TCP)**
```cpp
struct GameStatePacket {
    UUID playerID;
    gameMatrix matrix;      // Full board state
    int score;
    bag bag[7];         // Next tetromino sequence ?
    PowerUp powerUps[4];    // Active power-ups
    SpectatorView otherPlayers[8]; // Other players' boards
};
```

### **API Endpoints (Master Server)**
#### **Database Related Endpoints (DB Server)**
| Endpoint               | Method | Parameters                                                  | Purpose                                      | Done |
|------------------------|--------|-------------------------------------------------------------|----------------------------------------------|------|
| `/login`               | POST   | `userName`, `password`                                      | Authenticates a player                       | ✅    |
| `/register`            | POST   | `userName`, `password`                                      | Registers a new player                       | ✅    |
| `/update`              | POST   | `accountID`, `newName (optional)`, `newPassword (optional)` | Updates player details                       | ✅    |
| `/get_leaderboard`     | GET    | None                                                        | Retrieves the leaderboard                    | ❌    |
| `/get_player`          | GET    | `accountID`                                                 | Retrieves player profile                     | ✅    |
| `/get_friends`         | GET    | `accountID` (can be done via `/get_player`)                 | Fetches the list of friends for a given user | ❓    |
| `/send_friend_request` | POST   | `accountID`, `otherAccountID`                               | Sends a friend request                       | ❌    |
| `/add_friend`          | POST   | `accountID`, `otherAccountID`                               | Adds a player to friend list                 | ✅    |
| `/remove_Friend`       | POST   | `accountID`, `otherAccountID`                               | Removes a friend relation between two users  | ❌    |

#### **Messaging Endpoints (DB Server)**
| Endpoint          | Method | Parameters                                      | Purpose                                      | Done |
|-------------------|--------|-------------------------------------------------|----------------------------------------------|------|
| `/get_messages`   | GET    | `accountID`, `otherAccountID`                   | Fetches messages exchanged between two users | ❌    |
| `/post_message`   | POST   | `accountID`, `otherAccountID`, `messageContent` | Logs a new message                           | ❌    |
| `/delete_message` | POST   | `messageID`                                     | Deletes a message                            | ❌    |

#### **Lobby & Matchmaking Endpoints (Game Server)**
| Endpoint          | Method | Parameters                                 | Purpose                                  | Done |
|-------------------|--------|--------------------------------------------|------------------------------------------|------|
| `/create_lobby`   | POST   | `hostAccountID`, `gameMode`, `playerCount` | Creates a new lobby                      | ❌    |
| `/get_lobby`      | GET    | `lobbyID`                                  | Fetches lobby details                    | ❌    |
| `/invite`         | POST   | `accountID`, `otherAccountID`, `lobbyID`   | Joins an existing lobby                  | ❌    |
| `/join_lobby`     | POST   | `accountID`, `lobbyID`                     | Joins an existing lobby                  | ❌    |
| `/leave_lobby`    | POST   | `accountID`, `lobbyID`                     | Leaves a lobby                           | ❌    |
| `/spectate_lobby` | POST   | `accountID`, `lobbyID`                     | Allows a user to spectate a lobby        | ❌    |
| `/start_game`     | POST   | `lobbyID`                                  | Starts a match with players in the lobby | ❌    |

#### **In-Game Endpoints (Game Server)**
| Endpoint      | Method | Parameters                      | Purpose                          | Done |
|---------------|--------|---------------------------------|----------------------------------|------|
| `/get_game`   | GET    | `gameID`, `lobbyID`             | Retrieves the current game state | ❌    |
| `/send_event` | POST   | `accountID`, `lobbyID`, `event` | Submits a move to the GameEngine | ❌    |

## Additional Notes
- The database server strictly handles persistent data and forwards it to the master server.
- The game server ensures real-time gameplay responsiveness with efficient networking.
- The master server is responsible for matchmaking, authentication, and session management.