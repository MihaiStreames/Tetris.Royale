from ..db_script import TetrisDatabase
from ..account import Account
from ..common import hash_password


class PlayerService:

    """
    This class is responsible for handling the Database services that require user authentication.
    This includes the friend management, messaging, and account update services.
    """

    def __init__(self, db: TetrisDatabase, account: Account) -> None:
        self.db: TetrisDatabase = db
        self.account: Account = account

    def add_friend(self, friend_uid: int) -> None:
        
        """
        Adds a friend to the user's friend list.
        """

        self.db.post_friend(p1_uid=self.account.uid, p2_uid=friend_uid)

    def send_message(self, receiver_uid: int, message: str) -> None:
        
        """
        Sends a message to a friend using the receiver's uid.
        """

        self.db.post_message(
            sender_uid=self.account.uid, receiver_uid=receiver_uid, message=message
        )

    def get_friends(self) -> list[tuple[int, str]] | None:
        
        """
        Fetches the friends of the user.
        Returns a list of tuples, where each tuple contains the uid and username of a friend.
        If the user has no friends, None is returned.
        """

        friends: list[int] = self.db.get_friend_list(uid=self.account.uid)
        if friends is None:
            print("[!] No friends found")
            return None
        else:
            return [(uid, self.db.get_username(uid=uid)) for uid in friends]

    def get_messages_with_friend(self, friend_uid: int) -> list[dict]:
        
        """
        Fetches the messages between the user and a friend.
        Returns a list of dictionaries, where each dictionary contains
        the sender's uid, receiver's uid, the message, and the timestamp of a message.
        """

        return self.db.get_message_between_friends(
            p1_uid=self.account.uid, p2_uid=friend_uid
        )

    def update_username(self, new_username: str) -> bool:
        
        """
        Updates the username of the user.
        Returns True if the username was updated successfully, False otherwise.
        """

        could_update: bool = self.db.update_username(uid=self.account.uid, new_username=new_username):
        
        if could_update:
            self.account.username = new_username

        return could_update

    def update_password(self, new_password: str) -> bool:

        """
        Updates the password of the user.
        Returns True if the password was updated successfully, False otherwise.
        """

        could_update: bool = self.db.update_password(
            uid=self.account.uid, new_password_hash=hash_password(new_password)
        )

        if could_update:
            self.account.password = hash_password(new_password)

        return could_update

    def update_score(self, new_score: int) -> None:

        """
        Updates the score of the user.
        """

        self.db.post_score(uid=self.account.uid, score=new_score)
        self.account.score = new_score


