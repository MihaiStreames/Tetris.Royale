#!/bin/bash

# Base directory (relative to script location)
BASE_DIR="../../"

# Acronyms to preserve
ACRONYMS=("DB" "API" "HTTP" "TCP" "UDP" "Tetris" "Game" "Engine" "Server" "State" "Request" "Response" "Factory" "Matrix")

# Function to convert filename to PascalCase while preserving known acronyms
to_pascal_case() {
    local name="$1"
    local extension="$2"

    # Convert to PascalCase:
    local pascal_case=$(echo "$name" | awk -F'[^a-zA-Z0-9]+' '{
        for (i = 1; i <= NF; i++) {
            if ($i ~ /^[A-Z0-9]+$/) {
                printf "%s", $i;
            } else {
                printf "%s", toupper(substr($i,1,1)) tolower(substr($i,2));
            }
        }
    }')

    # Restore known acronyms
    for ACRONYM in "${ACRONYMS[@]}"; do
        pascal_case=$(echo "$pascal_case" | sed -E "s/(${ACRONYM,,})/\U\1/g")
    done

    echo "$pascal_case.$extension"
}

# Find all .cpp and .hpp files in include/ and src/, ignoring cmake-build-* folders
FILES=$(find "$BASE_DIR" -type f \( -path "*/include/*" -o -path "*/src/*" \) \
    -regex '.*\.\(cpp\|hpp\)$' ! -path "*/cmake-build-*/*")

echo "Found files in include/ and src/ (excluding cmake-build-*):"
echo "$FILES"
echo ""

# Rename files if needed
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
    fi
done

# Find updated files after renaming
FILES=$(find "$BASE_DIR" -type f \( -path "*/include/*" -o -path "*/src/*" \) \
    -regex '.*\.\(cpp\|hpp\)$' ! -path "*/cmake-build-*/*")

echo ""
echo "Applying clang-format..."

# Verify .clang-format exists
CLANG_FORMAT_CONFIG=".clang-format"
if [[ ! -f "$CLANG_FORMAT_CONFIG" ]]; then
    echo "❌ Error: .clang-format not found in $BASE_DIR"
    exit 1
fi

# Apply clang-format and check for changes
for FILE in $FILES; do
    clang-format -i "$FILE"
    echo "Formatted: $FILE"
    # Ensure clang-format actually modifies files
    if ! git diff --quiet "$FILE"; then
        echo "✅ clang-format modified: $FILE"
    else
        echo "⚠️ No changes needed for: $FILE"
    fi
done

echo "✅ Renaming and formatting complete."
