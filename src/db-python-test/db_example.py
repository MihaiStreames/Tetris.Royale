from db import TetrisDatabase, GameService, PlayerService, Account
import os

# This is the main entry point for the database module, with some example usage.


if __name__ == "__main__":

    if os.path.exists("tetris.db"):
        os.remove("tetris.db")

    # First, we create a DB object, which will be used by the services.
    # I'm not too much of a fan of curl, so I think having the database object stored locally is a good idea.
    # However, we could very well use MARIA-DB or some other db service to ensure that users are able to access the database from anywhere.
    db: TetrisDatabase = TetrisDatabase(dbname="tetris.db", load_on_init=True)

    # Next, we create a GameService object, which will be used to interact with the database.
    # Each client will have their own GameService object, which will be used to interact with the database.
    game_service: GameService = GameService(db=db)

    # Now, we can use the GameService object to register a new account / do some stuff (that doesn't require an account).
    # Let's register new accounts.
    account1: Account = game_service.register(username="ant0in", password="password123")
    account2: Account = game_service.register(username="bob", password="yayvideogames")
    game_service.register(username="zangdaar", password="secret")

    # Account 3 didn't get assigned to a variable, but that's fine. We can still use it by logging in.
    account3: Account = game_service.login(username="zangdaar", password="secret")

    # Now, let's create PlayerService objects, which will be used to interact with the database (when logged).
    player1_service: PlayerService = PlayerService(db=db, account=account1)
    player2_service: PlayerService = PlayerService(db=db, account=account2)
    player3_service: PlayerService = PlayerService(db=db, account=account3)

    # Now, we can use the PlayerService object to do some stuff (that requires an account).
    # Let's add a friends.
    player1_service.add_friend(friend_uid=account2.uid)  # ant0in adds bob
    player1_service.add_friend(friend_uid=account3.uid)  # ant0in adds zangdaar

    # Let's send a message!
    player1_service.send_message(receiver_uid=account2.uid, message="Hello, bob!")
    player2_service.send_message(receiver_uid=account1.uid, message="Hi, ant0in!")

    # Let's update the username of account1.
    player1_service.update_username(new_username="antoine")

    # Let's update the password of account1.
    player1_service.update_password(new_password="newpassword")

    # Let's update the score of account1.
    player1_service.update_score(new_score=100)

    # Let's get the friends of account1.
    friends: list[Account] = player1_service.get_friends()
    print(friends)

    # Let's get the messages between account1 and account2.
    messages: list[dict] = player1_service.get_messages_with_friend(
        friend_uid=account2.uid
    )
    print(messages)

    # Let's get the leaderboard.
    leaderboard: list[dict] = game_service.get_leaderboard()
    print(leaderboard)
