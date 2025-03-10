#!/bin/bash

# Base directory (relative to script location)
BASE_DIR="../../"

# List of acronyms to preserve their uppercase form
ACRONYMS=("DB" "API" "HTTP" "TCP" "UDP" "Tetris" "Game" "Engine" "Server" "State" "Request" "Response" "Factory" "Matrix")

# Function to convert filename to PascalCase while preserving known acronyms
to_pascal_case() {
    local name="$1"
    local extension="$2"

    # Split the filename into words
    local words=($(echo "$name" | sed -E 's/([^a-zA-Z0-9]+)/ /g'))
    local result=""

    for word in "${words[@]}"; do
        # Check if the word is an acronym
        for ACRONYM in "${ACRONYMS[@]}"; do
            if [[ "${word,,}" == "${ACRONYM,,}" ]]; then
                word="$ACRONYM"
                break
            fi
        done

        # Convert to PascalCase if not an acronym
        if [[ ! " ${ACRONYMS[@]} " =~ " ${word} " ]]; then
            word="$(tr '[:lower:]' '[:upper:]' <<< ${word:0:1})${word:1}"
        fi

        result+="$word"
    done

    echo "$result.$extension"
}

# Find all .hpp and .cpp files in include/ and src/, ignoring cmake-build-* folders
FILES=$(find "$BASE_DIR" -type f \( -path "*/include/*" -o -path "*/src/*" \) \
    -regex '.*\.\(cpp\|hpp\)$' ! -path "*/cmake-build-*/*")

echo "Found files in include/ and src/ (excluding cmake-build-*):"
echo "$FILES"
echo ""

# Step 1: Cache header file mappings BEFORE renaming
declare -A HEADER_MAP
for FILE in $FILES; do
    EXTENSION="${FILE##*.}"
    if [[ "$EXTENSION" == "hpp" ]]; then
        FILENAME="${FILE##*/}"
        HEADER_MAP["$FILENAME"]="$FILE"
    fi
done

# Step 2: Rename files if needed
declare -A RENAMED_HEADERS
for FILE in $FILES; do
    DIR=$(dirname "$FILE")
    BASENAME=$(basename "$FILE")
    EXTENSION="${BASENAME##*.}"
    FILENAME="${BASENAME%.*}"

    # Convert to PascalCase
    NEW_NAME=$(to_pascal_case "$FILENAME" "$EXTENSION")

    if [[ "$BASENAME" != "$NEW_NAME" ]]; then
        echo "Renaming: $BASENAME → $NEW_NAME"
        mv "$FILE" "$DIR/$NEW_NAME"
        if [[ "$EXTENSION" == "hpp" ]]; then
            RENAMED_HEADERS["$BASENAME"]="$NEW_NAME"
        fi
    fi
done

# Step 3: Update `#include` statements in all files
for FILE in $FILES; do
    if [[ "$FILE" =~ \.(cpp|hpp)$ ]]; then
        for OLD_HEADER in "${!RENAMED_HEADERS[@]}"; do
            NEW_HEADER="${RENAMED_HEADERS[$OLD_HEADER]}"
            OLD_HEADER_PATH="${HEADER_MAP[$OLD_HEADER]}"
            if [[ -n "$OLD_HEADER_PATH" ]]; then
                echo "Updating includes in: $FILE"
                sed -i "s|#include \"$OLD_HEADER\"|#include \"$NEW_HEADER\"|g" "$FILE"
            fi
        done
    fi
done

echo ""
echo "Applying clang-format..."

# Verify .clang-format exists
CLANG_FORMAT_CONFIG=".clang-format"
if [[ ! -f "$CLANG_FORMAT_CONFIG" ]]; then
    echo "❌ Error: .clang-format not found in $BASE_DIR"
    exit 1
fi

# Apply clang-format
for FILE in $FILES; do
    clang-format -i "$FILE"
    echo "Formatted: $FILE"
done

echo "✅ Renaming, header propagation, and formatting complete."
