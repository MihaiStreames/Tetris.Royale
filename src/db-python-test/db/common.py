import hashlib


def hash_password(password: str) -> str:
    return hashlib.sha256(password.encode()).hexdigest()
