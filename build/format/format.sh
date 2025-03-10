#!/bin/bash

BASE_DIR="../../"

# Function to convert filename to PascalCase while preserving acronyms
to_pascal_case() {
    local name="$1"
    local extension="$2"
    
    # Convert to PascalCase: 
    # - Split on non-alphanumeric characters
    # - Capitalize first letter of each word
    # - Preserve known acronyms like "DB", "API"
    
    local pascal_case=$(echo "$name" | awk -F'[^a-zA-Z0-9]+' '{
        for (i = 1; i <= NF; i++) {
            # Preserve uppercase words
            if ($i ~ /^[A-Z0-9]+$/) {
                printf "%s", $i;
            } else {
                printf "%s", toupper(substr($i,1,1)) tolower(substr($i,2));
            }
        }
    }')

    echo "$pascal_case.$extension"
}

# Find all .cpp and .hpp files in include/ and src/, ignoring cmake-build-* folders
FILES=$(find "$BASE_DIR" -type f \( -path "*/include/*" -o -path "*/src/*" \) \
    -regex '.*\.\(cpp\|hpp\)$' ! -path "*/cmake-build-*/*")

echo "Found files:"
echo "$FILES"format
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
FILES=$(find . -regex '.*\.\(cpp\|hpp\)$')

echo ""
echo "Applying clang-format..."

# Apply clang-format
for FILE in $FILES; do
    clang-format -i "$FILE"
    echo "Formatted: $FILE"
done

echo "✅ Renaming and formatting complete."
