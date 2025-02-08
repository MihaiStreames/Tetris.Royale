from ..db_script import TetrisDatabase
from ..account import Account
from ..common import hash_password


class GameService:
    """
    This class is responsible for handling the Database services that don't require any user authentication.
    This includes the login, registration, and leaderboard fetch services.
    """

    def __init__(self, db: TetrisDatabase) -> None:
        self.db: TetrisDatabase = db

    def login(self, username: str, password: str) -> Account | None:
        """
        Logs in a user with the given username and password.
        If the login is successful, the serialized account object is returned.
        If the login fails, None is returned.
        """

        hashed_password: str = hash_password(password)
        uid: int | None = self.db.get_uid_from_username_and_password_hash(
            username=username, password_hash=hashed_password
        )

        if uid is None:
            print("Login failed")
            return None

        return Account.deserialize(self.db.get_account(uid=uid))

    def register(self, username: str, password: str) -> Account | None:
        """
        Registers a new user with the given username and password.
        If the registration is successful, the serialized account object is returned.
        If the registration fails, None is returned.
        """

        hashed_password = hash_password(password)
        uid: int | None = self.db.post_account(
            username=username, password_hash=hashed_password
        )

        if uid is None:
            print("Registration failed")
            return None

        return Account.deserialize(self.db.get_account(uid=uid))

    def get_leaderboard(self) -> list[tuple[str, int]]:
        """
        Fetches the leaderboard from the database.
        Returns a list of tuples, where each tuple contains the username and score of a user.
        """

        return [
            (self.db.get_username(uid=uid), self.db.get_score(uid=uid))
            for uid in self.db.get_leaderboard()
        ]
