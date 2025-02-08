# This file contains the classes and functions that are used to serialize and deserialize the data that is sent between the client and the server.


class Account:

    def __init__(
        self, uid: int, username: str, password_hash: str, best_score: int
    ) -> None:
        self.uid: int = uid
        self.username: str = username
        self.password_hash: str = password_hash
        self.best_score: int = best_score

    @classmethod
    def deserialize(cls, data: dict) -> None:
        return cls(
            uid=data["uid"],
            username=data["username"],
            password_hash=data["password_hash"],
            best_score=data["best_score"],
        )

    def serialize(self) -> dict:
        return {
            "uid": self.uid,
            "username": self.username,
            "password_hash": self.password_hash,
            "best_score": self.best_score,
        }

    def __repr__(self) -> str:
        return f"Account(uid={self.uid}, username={self.username}, password_hash={self.password_hash}, best_score={self.best_score})"
