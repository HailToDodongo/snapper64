set -e

TESTS=$(sc64deployer sd ls tests | awk '{print $6}')
TARGET_DIR="assets"

for TEST in $TESTS; do
    # check if already downloaded
    if [ -f "$TARGET_DIR/$TEST.7z" ]; then
        echo "$TEST already downloaded, skipping."
        continue
    fi
    echo "Downloading $TEST to $TARGET_DIR/$TEST"
    sc64deployer sd download "tests/$TEST" "$TARGET_DIR/$TEST"

    echo "Compressing $TEST to $TARGET_DIR/$TEST.7z"
    7z a -t7z -mx=9 "$TARGET_DIR/$TEST.7z" "$TARGET_DIR/$TEST" > /dev/null

    echo "Removing uncompressed $TEST"
    rm -rf "$TARGET_DIR/$TEST"

    # echo "Removing SD card copy of $TEST"
    # sc64deployer sd rm "tests/$TEST"
done

echo "All tests downloaded to $TARGET_DIR"