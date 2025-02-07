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
| Component | Protocol | Purpose |
|-----------|---------|---------|
| **Master Server** | **TCP (HTTP/WebSockets)** | Lobby management, matchmaking, database queries |
| **Game Server (During Match)** | **UDP (keystrokes)** | Fast input handling |
| | **TCP (game state updates)** | Full board state, scores, power-ups |
| **Clients** | **TCP (persistent connection)** | Receiving game states |
| **Spectators** | **TCP only** | Watching matches in real time |

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

## API Endpoints (Master Server)
### **Authentication & Player Management**
| Endpoint | Method | Parameters | Purpose |
|----------|--------|------------|---------|
| `/login` | POST | `name`, `password` | Authenticates a player |
| `/register` | POST | `name`, `password` | Registers a new player |
| `/getPlayer` | GET | `name` | Retrieves player profile |
| `/update` | POST | `name`, `password (optional)` | Updates player details |
| `/addFriend` | POST | `name`, `friendName` | Adds a player to friend list |

### **Lobby & Matchmaking (TBD)**
| Endpoint                | Method | Parameters            | Purpose |
|-------------------------|--------|-----------------------|---------|
| `/createLobby`          | POST | `hostPlayerID`        | Creates a new lobby |
| `/joinLobby`            | POST | `playerID`, `lobbyID` | Joins an existing lobby |
| `/leaveLobby`           | POST | `playerID`, `lobbyID` | Leaves a lobby |
| `/startGame` (optional) | POST | `lobbyID`             | Starts a match with players in the lobby |

### **Gameplay Communication**
- **Keystrokes via UDP** (low latency, high-speed input handling).
- **Full state updates via TCP** (ensuring correctness and reliability).
- **Spectator updates via TCP** (real-time view of other players).

## Session Handling
- **Each player is assigned a session on the game server**.
- A **persistent TCP connection** is maintained for the game duration.
- A **UDP socket is used for keystrokes**.
- Spectators receive **TCP state updates**.
- When a game ends, the session is **closed**.