set -e

TESTS=$(sc64deployer sd ls tests | awk '{print $6}')
TARGET_DIR="assets"

for TEST in $TESTS; do
    echo "Removing SD card copy of $TEST"
    sc64deployer sd rm "tests/$TEST"
done

echo "All tests downloaded to $TARGET_DIR"