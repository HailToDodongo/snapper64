#!/bin/bash

DIR="${1:-.}"

if [[ ! -d "$DIR" ]]; then
    echo "Error: '$DIR' is not a directory."
    exit 1
fi

total=0

shopt -s nullglob
for f in "$DIR"/*.7z; do
    size=$(7z l "$f" 2>/dev/null | awk 'END {print $3}')

    size=${size:-0}
    echo "Size of $(basename "$f"): $size bytes"

    total=$((total + size))
done

echo "--------------------------------------"
echo "Total uncompressed size: $total bytes"