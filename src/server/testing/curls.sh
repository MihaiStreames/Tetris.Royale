#!/bin/bash
# test_db_endpoints.sh
# This script tests the DB endpoints via the master server (assumed to be running on port 8080).

# Helper function to print a separator.
separator() {
    echo "------------------------------------------------------------"
}

# 1. Register Alice
separator
echo "Registering Alice..."
curl -X POST -H "Content-Type: application/json" \
  -d '{"name": "Alice", "password": "secret"}' \
  http://localhost:8080/register
echo -e "\n"

# 2. Register Bob
separator
echo "Registering Bob..."
curl -X POST -H "Content-Type: application/json" \
  -d '{"name": "Bob", "password": "secret"}' \
  http://localhost:8080/register
echo -e "\n"

# 3. Log in as Alice
separator
echo "Logging in as Alice..."
curl -X POST -H "Content-Type: application/json" \
  -d '{"name": "Alice", "password": "secret"}' \
  http://localhost:8080/login
echo -e "\n"

# 4. Update Alice's username and password (to Alice2 and newsecret)
separator
echo "Updating Alice to Alice2..."
curl -X POST -H "Content-Type: application/json" \
  -d '{"name": "Alice", "password": "secret", "newName": "Alice2", "newPassword": "newsecret"}' \
  http://localhost:8080/update
echo -e "\n"

# 5. Add Bob as a friend for Alice2 (note: no password needed for friend addition)
separator
echo "Alice2 adding Bob as a friend..."
curl -X POST -H "Content-Type: application/json" \
  -d '{"name": "Alice2", "friendName": "Bob"}' \
  http://localhost:8080/add_friend
echo -e "\n"

# 6. Get info for Alice2
separator
echo "Getting player info for Alice2..."
curl -X GET http://localhost:8080/get_player?name=Alice2
echo -e "\n"

# 7. Get info for Bob
separator
echo "Getting player info for Bob..."
curl -X GET http://localhost:8080/get_player?name=Bob
echo -e "\n"