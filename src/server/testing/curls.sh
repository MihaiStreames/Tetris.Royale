#!/usr/bin/env bash

# Check if jq is installed (used to parse JSON)
if ! command -v jq &> /dev/null
then
    echo "ERROR: 'jq' is not installed. Install and re-run"
    exit 1
fi

separator() {
    echo "------------------------------------------------------------"
}

# 1. Register 'Alice'
separator
echo "Registering Alice..."
resp=$(curl -s -X POST -H "Content-Type: application/json" \
  -d '{"userName": "Alice", "password": "secret"}' \
  http://localhost:8080/register)

echo "Response: $resp"
aliceID=$(echo "$resp" | jq -r '.accountID')
echo "aliceID = $aliceID"

# 2. Register 'Bob'
separator
echo "Registering Bob..."
resp=$(curl -s -X POST -H "Content-Type: application/json" \
  -d '{"userName": "Bob", "password": "secret"}' \
  http://localhost:8080/register)

echo "Response: $resp"
bobID=$(echo "$resp" | jq -r '.accountID')
echo "bobID = $bobID"

# 3. Log in as Alice (by userName/password)
separator
echo "Logging in as Alice..."
curl -X POST -H "Content-Type: application/json" \
  -d '{"userName": "Alice", "password": "secret"}' \
  http://localhost:8080/login
echo -e "\n"

# 4. Update Alice to 'Alice2' with a new password
separator
echo "Updating Alice => Alice2 (newsecret)"
curl -X POST -H "Content-Type: application/json" \
  -d "{\"accountID\": \"$aliceID\", \"password\": \"secret\", \"newName\": \"Alice2\", \"newPassword\": \"newsecret\"}" \
  http://localhost:8080/update
echo -e "\n"

# 5. Send friend request from Alice2 -> Bob
separator
echo "Alice2 ($aliceID) sending friend request to Bob ($bobID)..."
curl -X POST -H "Content-Type: application/json" \
  -d "{\"accountID\": \"$aliceID\", \"otherAccountID\": \"$bobID\"}" \
  http://localhost:8080/send_friend_request
echo -e "\n"

# 6. Bob accepts the friend request (add_friend)
separator
echo "Bob ($bobID) accepting friend request from Alice2 ($aliceID)..."
curl -X POST -H "Content-Type: application/json" \
  -d "{\"accountID\": \"$bobID\", \"otherAccountID\": \"$aliceID\"}" \
  http://localhost:8080/add_friend
echo -e "\n"

# 7. Get player info for Alice2
separator
echo "Fetching player info for Alice2..."
curl -X GET "http://localhost:8080/get_player?accountID=$aliceID"
echo -e "\n"

# 8. Get player info for Bob
separator
echo "Fetching player info for Bob..."
curl -X GET "http://localhost:8080/get_player?accountID=$bobID"
echo -e "\n"

# 9. Bob removes Alice2 as a friend
separator
echo "Bob ($bobID) removing friend Alice2 ($aliceID)..."
curl -X POST -H "Content-Type: application/json" \
  -d "{\"accountID\": \"$bobID\", \"otherAccountID\": \"$aliceID\"}" \
  http://localhost:8080/remove_friend
echo -e "\n"

# 10. Get the leaderboard
separator
echo "Getting leaderboard..."
curl -X GET "http://localhost:8080/get_leaderboard"
echo -e "\n"

# 11. Attempt to fetch a non-existent account
separator
echo "Trying to fetch a player that doesn't exist..."
curl -X GET "http://localhost:8080/get_player?accountID=notarealID"
echo -e "\n"