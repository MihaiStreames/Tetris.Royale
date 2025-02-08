import sqlite3
import json
from datetime import datetime
import re


class TetrisDatabase:

    def __init__(self, dbname: str, load_on_init: bool = True) -> None:

        self._dbname: str = dbname
        if load_on_init:
            self.create_tables()

    @property
    def dbname(self) -> str:
        return self._dbname

    def connect_db(self) -> sqlite3.Connection:
        return sqlite3.connect(database=self.dbname)

    def get_cursor(self, conn: sqlite3.Connection) -> sqlite3.Cursor:
        return conn.cursor()

    def close_db(self, conn: sqlite3.Connection) -> None:
        conn.close()

    def commit_db(self, conn: sqlite3.Connection) -> None:
        conn.commit()

    def create_tables(self) -> None:

        conn: sqlite3.Connection = self.connect_db()
        cursor: sqlite3.Cursor = self.get_cursor(conn=conn)

        # Create the tables if they don't exist
        # I could think of a better way to do this, but for now, this will do
        # as I'm absolutely trashed right now lol

        cursor.execute(
            """
            CREATE TABLE IF NOT EXISTS players (
                uid INTEGER PRIMARY KEY AUTOINCREMENT,
                username TEXT UNIQUE NOT NULL,
                password_hash TEXT NOT NULL,
                best_score INTEGER DEFAULT 0
            )
        """
        )

        cursor.execute(
            """
            CREATE TABLE IF NOT EXISTS friends (
                player1_id INTEGER NOT NULL,
                player2_id INTEGER NOT NULL,
                PRIMARY KEY (player1_id, player2_id),
                FOREIGN KEY (player1_id) REFERENCES players(uid) ON DELETE CASCADE,
                FOREIGN KEY (player2_id) REFERENCES players(uid) ON DELETE CASCADE
            )
        """
        )

        cursor.execute(
            """
            CREATE TABLE IF NOT EXISTS messages (
                message_id INTEGER PRIMARY KEY AUTOINCREMENT,
                sender_id INTEGER NOT NULL,
                receiver_id INTEGER NOT NULL,
                message_content TEXT NOT NULL,
                timestamp TEXT DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (sender_id) REFERENCES players(uid) ON DELETE CASCADE,
                FOREIGN KEY (receiver_id) REFERENCES players(uid) ON DELETE CASCADE
            )
        """
        )

        self.commit_db(conn=conn)
        self.close_db(conn=conn)

    # utils

    @staticmethod
    def validate_username_string(username: str) -> bool:
        MIN_LENGTH: int = 3
        MAX_LENGTH: int = 20
        pattern = rf"^[a-zA-Z0-9]{{{MIN_LENGTH},{MAX_LENGTH}}}$"
        return bool(re.match(pattern, username))

    @staticmethod
    def validate_password_hash_string(password_hash: str) -> bool:
        MIN_LENGTH: int = 8
        MAX_LENGTH: int = 64
        pattern = rf"^[a-fA-F0-9]{{{MIN_LENGTH},{MAX_LENGTH}}}$"
        return bool(re.match(pattern, password_hash))

    def is_username_taken(self, username: str) -> bool:
        """
        Check if a username already exists in the database.
        Returns True if the username is taken, False otherwise.
        """

        conn: sqlite3.Connection = self.connect_db()
        cursor: sqlite3.Cursor = self.get_cursor(conn=conn)

        # we count the number of rows that have the same username
        # note that we could do this with a SELECT * FROM players WHERE username = ?
        # but we only need the count, so we can optimize this

        cursor.execute("SELECT COUNT(*) FROM players WHERE username = ?", (username,))
        result: any = cursor.fetchone()[0]

        self.close_db(conn=conn)

        return result > 0

    def get_uid_from_username_and_password_hash(
        self, username: str, password_hash: str
    ) -> int | None:
        """
        Get the uid of a player given their username and password hash.
        Returns the uid of the player if the username and password hash match, None otherwise.
        """

        conn: sqlite3.Connection = self.connect_db()
        cursor: sqlite3.Cursor = self.get_cursor(conn=conn)

        cursor.execute(
            "SELECT uid FROM players WHERE username = ? AND password_hash = ?",
            (username, password_hash),
        )

        result: any = cursor.fetchone()

        self.close_db(conn=conn)

        return result[0] if result else None

    # db post operations

    def post_account(self, username: str, password_hash: str) -> int | None:
        """
        Add a player account to the database.
        Returns the uid of the player account if the operation was successful, None otherwise.
        """

        # we want to validate the username and password_hash
        # by matching some regex pattern (e.g. alphanumeric / length)
        # and also check if the username is not already taken

        # check if the username / password_hash are valid
        if not self.validate_username_string(
            username
        ) or not self.validate_password_hash_string(password_hash):
            return None

        # check if the username is already taken
        if self.is_username_taken(username):
            return None

        conn: sqlite3.Connection = self.connect_db()
        cursor: sqlite3.Cursor = self.get_cursor(conn=conn)

        # Insérer le joueur
        cursor.execute(
            "INSERT INTO players (username, password_hash) VALUES (?, ?)",
            (username, password_hash),
        )

        new_uid: int = (
            cursor.lastrowid
        )  # 'lastrowid' contains the last inserted row's id (aka the uid)

        self.commit_db(conn=conn)
        self.close_db(conn=conn)

        return new_uid

    def post_friend(self, p1_uid: int, p2_uid: int) -> None:

        conn: sqlite3.Connection = self.connect_db()
        cursor: sqlite3.Cursor = self.get_cursor(conn=conn)

        cursor.execute(
            "INSERT INTO friends (player1_id, player2_id) VALUES (?, ?)",
            (p1_uid, p2_uid),
        )

        self.commit_db(conn=conn)
        self.close_db(conn=conn)

    def post_message(self, sender_uid: int, receiver_uid: int, message: str) -> None:

        conn: sqlite3.Connection = self.connect_db()
        cursor: sqlite3.Cursor = self.get_cursor(conn=conn)

        cursor.execute(
            "INSERT INTO messages (sender_id, receiver_id, message_content, timestamp) VALUES (?, ?, ?, ?)",
            (
                sender_uid,
                receiver_uid,
                json.dumps(obj=message),
                datetime.utcnow().isoformat(),
            ),
        )

        self.commit_db(conn=conn)
        self.close_db(conn=conn)

    def post_score(self, uid: int, score: int) -> None:

        conn: sqlite3.Connection = self.connect_db()
        cursor: sqlite3.Cursor = self.get_cursor(conn=conn)

        cursor.execute("UPDATE players SET best_score = ? WHERE uid = ?", (score, uid))

        self.commit_db(conn=conn)
        self.close_db(conn=conn)

    # db update operations

    def update_password(self, uid: int, new_password_hash: str) -> bool:

        if not self.validate_password_hash_string(new_password_hash):
            return False

        conn: sqlite3.Connection = self.connect_db()
        cursor: sqlite3.Cursor = self.get_cursor(conn=conn)

        cursor.execute(
            "UPDATE players SET password_hash = ? WHERE uid = ?",
            (new_password_hash, uid),
        )

        self.commit_db(conn=conn)
        self.close_db(conn=conn)

        return True

    def update_username(self, uid: int, new_username: str) -> bool:

        if not self.validate_username_string(new_username):
            return False

        if self.is_username_taken(new_username):
            return False

        conn: sqlite3.Connection = self.connect_db()
        cursor: sqlite3.Cursor = self.get_cursor(conn=conn)

        cursor.execute(
            "UPDATE players SET username = ? WHERE uid = ?", (new_username, uid)
        )

        self.commit_db(conn=conn)
        self.close_db(conn=conn)

        return True

    # db get operations

    def get_username(self, uid: int) -> str | None:
        """
        Get the username of a player.
        Returns the username of the player if it exists, None otherwise.
        """

        acc: dict | None = self.get_account(uid=uid)
        return acc["username"] if acc is not None else None

    def get_account(self, uid: int) -> dict | None:
        """
        Get a player account from the database.
        Returns a dictionary containing the player account if it exists, None otherwise.
        """

        conn: sqlite3.Connection = self.connect_db()
        cursor: sqlite3.Cursor = self.get_cursor(conn=conn)

        cursor.execute("SELECT * FROM players WHERE uid = ?", (uid,))
        result: any = cursor.fetchone()

        self.close_db(conn=conn)

        if result is not None:
            result: dict = {
                "uid": result[0],
                "username": result[1],
                "password_hash": result[2],
                "best_score": result[3],
            }

        return result

    def get_friend_list(self, uid: int) -> list[int]:
        """
        Get the friend list of a player.
        Returns a list of tuples containing the uid of the player's friends.
        """

        conn: sqlite3.Connection = self.connect_db()
        cursor: sqlite3.Cursor = self.get_cursor(conn=conn)

        cursor.execute("SELECT player2_id FROM friends WHERE player1_id = ?", (uid,))

        result: list[int] = [row[0] for row in cursor.fetchall()]

        self.close_db(conn=conn)

        return result

    def get_message_between_friends(self, p1_uid: int, p2_uid: int) -> list[dict]:
        """
        Get the messages between two friends.
        Returns a list of dictionaries containing the messages.
        """

        conn: sqlite3.Connection = self.connect_db()
        cursor: sqlite3.Cursor = self.get_cursor(conn=conn)

        cursor.execute(
            """
            SELECT message_content, timestamp
            FROM messages
            WHERE (sender_id = ? AND receiver_id = ?) OR (sender_id = ? AND receiver_id = ?)
        """,
            (p1_uid, p2_uid, p2_uid, p1_uid),
        )

        result: list[dict] = [
            {"message_content": row[0], "timestamp": row[1]}
            for row in cursor.fetchall()
        ]

        self.close_db(conn=conn)

        return result

    def get_leaderboard(self) -> list[int] | None:
        """
        Get the leaderboard.
        Returns a list of uid of players sorted by best_score.
        """

        conn: sqlite3.Connection = self.connect_db()
        cursor: sqlite3.Cursor = self.get_cursor(conn=conn)

        cursor.execute("SELECT uid FROM players ORDER BY best_score DESC")
        result: list[int] = [row[0] for row in cursor.fetchall()]

        self.close_db(conn=conn)

        return result

    def get_score(self, uid: int) -> int | None:
        """
        Get the best score of a player.
        Returns the best score of the player if it exists, None otherwise.
        """

        acc: dict | None = self.get_account(uid=uid)
        return acc["best_score"] if acc is not None else None
