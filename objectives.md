# **DBServer**

## **Access:**
- Has access to DB.

## **Handles:**
- **Requests GET to DB:**  
  Retrieves data from the database (e.g., getting the leaderboard).
  
- **Requests POST to DB:**  
  Updates data in the database (e.g., updating the user's best score using UID).

## **Requests:**
- **GET_FRIENDS [UID]** -> `List[Tuple[UID, USR_NAME]]`  
  *Fetches the list of friends for a given user (UID).*
  
- **ADD_FRIEND [UID1, UID2]** -> `...`  
  *Adds a friend relation between two users (UID1, UID2).*
  
- **REMOVE_FRIEND [UID1, UID2]** -> `...`  
  *Removes a friend relation between two users (UID1, UID2).*
  
- **SEND_FRIEND_REQUEST [UID1, UID2]** -> `...`  
  *Sends a friend request from UID1 to UID2.*

- **GET_MSG_WITH_FRIEND [UID1, UID2]** -> `List[MSG]`  
  *Fetches the messages exchanged between two users (UID1 and UID2).  
  Each message contains Timestamp, sender, receiver, and content.*

- **POST_MSG [UID1, UID2, CONTENT]** -> `...`  
  *Logs a new message from UID1 to UID2 with the provided content.  
  Do not use this request in any other way than logging messages.*

- **GET_LEADERBOARD []** -> `List[Tuple[USR_NAME, SCORE]]`  
  *Retrieves the leaderboard, containing user names and scores.*

- **LOGIN [USR_NAME, PWD_HASH]** -> `UID | null`  
  *Attempts to log in a player. If successful, returns the player's UID, otherwise returns null.*

- **REGISTER [USR_NAME, PWD_HASH]** -> `UID | null`  
  *Registers a new user. If successful, returns a new UID; if the username is taken, returns null.*

- **CHANGE_USRNAME [UID, USR_NAME]** -> `...`  
  *Allows a user to change their username (UID).*

- **CHANGE_PASSWORD [UID, PWD_HASH]** -> `...`  
  *Allows a user to change their password (UID).*

- **IS_USR_NAME_TAKEN [USR_NAME]** -> `bool`  
  *Checks if the given username is already taken in the database.*

---

# **GameServer**

## **Access:**
- Has access to all lobbies.
- Has access to all active games.

## **Handles:**
- **User 'POST' requests:**  
  Identifies the game that contains the player issuing the command and forwards it to the GameEngine (e.g., `ROTATE_LEFT`).
  
- **User 'GET' requests:**  
  Retrieves the current state of the game for the player, used by the client-side GUI.

## **Requests:**

### **Lobby Stuff:**
- **GET_LOBBY_INFO [LOBBY_ID]** -> `map[]`  
  *Fetches all the information about a lobby, including players, game mode, number of empty slots, and other details.*

- **GET_USER_LOBBY [UID]** -> `LOBBY_ID | null`  
  *Retrieves the lobby ID of the user (UID). If not in a lobby, returns null.*

- **JOIN_LOBBY [UID, LOBBY_ID]** -> `...`  
  *Allows a user to join a specific lobby.*

- **SPECTATE_LOBBY [UID, LOBBY_ID]** -> `...`  
  *Adds a player (UID) to the list of spectators in the given lobby.*

### **Game Stuff:**
- **PLAY_MOVE [UID, LOBBY_ID, MOVE]** -> `...`  
  *Forwards the move made by a player (UID) to the corresponding GameEngine, provided they are in the lobby.*

- **GET_GAME [UID, LOBBY_ID]** -> `GAMESTATE`  
  *Retrieves the current state of the game for a player in the specified lobby.*

---

# **MasterServer**

## **Access:**
- Has access to DBServer.
- Has access to GameServer.

## **Handles:**
- Forwards DB requests to DBServer.
- Forwards Game requests to GameServer.
