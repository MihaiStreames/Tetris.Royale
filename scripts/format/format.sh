#!/bin/bash

set -e  # Exit immediately if a command exits with a non-zero status

# Base directory (relative to script location)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BASE_DIR="$(realpath "$SCRIPT_DIR/../../")"
cd "$BASE_DIR"

# List of acronyms to preserve their uppercase form
ACRONYMS=("DB" "API" "HTTP" "TCP" "UDP" "Tetris" "Game" "Engine" "Server" "State" "Request" "Response" "Factory" "Matrix")

# Check if clang-format is installed
if ! command -v clang-format &> /dev/null; then
    echo "❌ Error: clang-format is not installed. Please install it first."
    exit 1
fi

# Check if .clang-format exists (first in script dir, then in base dir)
CLANG_FORMAT_PATH=""
if [[ -f "$SCRIPT_DIR/.clang-format" ]]; then
    CLANG_FORMAT_PATH="$SCRIPT_DIR/.clang-format"
    echo "Found .clang-format in script directory."
elif [[ -f "$BASE_DIR/.clang-format" ]]; then
    CLANG_FORMAT_PATH="$BASE_DIR/.clang-format"
    echo "Found .clang-format in base directory."
else
    echo "❌ Error: .clang-format not found in script or base directory."
    exit 1
fi

# Function to convert filename to PascalCase while preserving known acronyms
to_pascal_case() {
    local name="$1"
    local extension="$2"

    # Split the filename into words
    local words=($(echo "$name" | sed -E 's/([^a-zA-Z0-9]+)/ /g'))
    local result=""

    for word in "${words[@]}"; do
        # Check if the word is an acronym
        is_acronym=false
        for ACRONYM in "${ACRONYMS[@]}"; do
            if [[ "${word,,}" == "${ACRONYM,,}" ]]; then
                word="$ACRONYM"
                is_acronym=true
                break
            fi
        done

        # Convert to PascalCase if not an acronym
        if [[ "$is_acronym" == "false" ]]; then
            word="$(tr '[:lower:]' '[:upper:]' <<< ${word:0:1})${word:1}"
        fi

        result+="$word"
    done

    echo "$result.$extension"
}

# Find all .hpp and .cpp files in include/ and src/, ignoring cmake-build-* folders
FILES=$(find "$BASE_DIR" -type f \( -path "*/include/*" -o -path "*/src/*" \) \
    -regex '.*\.\(cpp\|hpp\)$' ! -path "*/cmake-build-*/*" | sort)

echo "Found $(echo "$FILES" | wc -l) files in include/ and src/ (excluding cmake-build-*):"
echo "$FILES" | sed 's/^/  /'
echo ""

# Process headers first, then source files
echo "Step 1: Processing headers and tracking their usage..."
declare -A HEADER_USAGE
declare -A RENAME_MAP

# First build the mapping of header usage
for FILE in $FILES; do
    for OTHER_FILE in $FILES; do
        if [[ "$FILE" != "$OTHER_FILE" ]]; then
            BASENAME=$(basename "$OTHER_FILE")
            if grep -q "#include \"$BASENAME\"" "$FILE"; then
                if [[ -z "${HEADER_USAGE[$BASENAME]}" ]]; then
                    HEADER_USAGE[$BASENAME]="$FILE"
                else
                    HEADER_USAGE[$BASENAME]+=" $FILE"
                fi
            fi
        fi
    done
done

echo "Header usage map:"
for HEADER in "${!HEADER_USAGE[@]}"; do
    COUNT=$(echo "${HEADER_USAGE[$HEADER]}" | wc -w)
    echo "  $HEADER is used in $COUNT files"
done
echo ""

# Process headers first, then source files, to ensure proper propagation
echo "Step 2: Renaming files and updating includes..."

# Process all header files first
for FILE in $FILES; do
    if [[ "$FILE" =~ \.hpp$ ]]; then
        DIR=$(dirname "$FILE")
        BASENAME=$(basename "$FILE")
        EXTENSION="${BASENAME##*.}"
        FILENAME="${BASENAME%.*}"

        # Convert to PascalCase
        NEW_NAME=$(to_pascal_case "$FILENAME" "$EXTENSION")

        if [[ "$BASENAME" != "$NEW_NAME" ]]; then
            echo "Renaming header: $BASENAME → $NEW_NAME"
            RENAME_MAP["$BASENAME"]="$NEW_NAME"

            # Keep track of the new file path for future reference
            NEW_FILE="$DIR/$NEW_NAME"

            # Rename the file first
            mv "$FILE" "$NEW_FILE"

            # Update all files that include this header
            if [[ -n "${HEADER_USAGE[$BASENAME]}" ]]; then
                for USAGE_FILE in ${HEADER_USAGE[$BASENAME]}; do
                    # Check if the usage file still exists (might have been renamed already)
                    REAL_USAGE_FILE="$USAGE_FILE"
                    BASE_USAGE=$(basename "$USAGE_FILE")
                    if [[ -n "${RENAME_MAP[$BASE_USAGE]}" ]] && [[ ! -f "$USAGE_FILE" ]]; then
                        # Try to find the renamed file
                        REAL_USAGE_FILE="$(dirname "$USAGE_FILE")/${RENAME_MAP[$BASE_USAGE]}"
                    fi

                    if [[ -f "$REAL_USAGE_FILE" ]]; then
                        echo "  Updating #include in: $(basename "$REAL_USAGE_FILE")"
                        sed -i "s|#include \"$BASENAME\"|#include \"$NEW_NAME\"|g" "$REAL_USAGE_FILE"
                    else
                        echo "  Warning: Could not find file: $USAGE_FILE (or its renamed version)"
                    fi
                done
            fi
        fi
    fi
done

# Now process all source files
for FILE in $FILES; do
    if [[ "$FILE" =~ \.cpp$ ]]; then
        DIR=$(dirname "$FILE")
        BASENAME=$(basename "$FILE")
        EXTENSION="${BASENAME##*.}"
        FILENAME="${BASENAME%.*}"

        # Convert to PascalCase
        NEW_NAME=$(to_pascal_case "$FILENAME" "$EXTENSION")

        if [[ "$BASENAME" != "$NEW_NAME" ]]; then
            echo "Renaming source: $BASENAME → $NEW_NAME"
            mv "$FILE" "$DIR/$NEW_NAME"
        fi
    fi
done

echo ""
echo "Step 3: Applying clang-format..."

# Find all files again (including renamed ones)
UPDATED_FILES=$(find "$BASE_DIR" -type f \( -path "*/include/*" -o -path "*/src/*" \) \
    -regex '.*\.\(cpp\|hpp\)$' ! -path "*/cmake-build-*/*" | sort)

# Apply clang-format using the detected config
for FILE in $UPDATED_FILES; do
    clang-format -style=file:"$CLANG_FORMAT_PATH" -i "$FILE"
    echo "Formatted: $(basename "$FILE")"
done

echo "✅ File renaming, header propagation, and formatting complete."
echo "   Total files processed: $(echo "$UPDATED_FILES" | wc -l)"

# Summary of changes made
if [[ ${#RENAME_MAP[@]} -gt 0 ]]; then
    echo ""
    echo "Summary of renamed headers:"
    for OLD_NAME in "${!RENAME_MAP[@]}"; do
        echo "  $OLD_NAME → ${RENAME_MAP[$OLD_NAME]}"
    done
fi