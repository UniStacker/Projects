#!/bin/bash
# File: test_extract.sh
# Description: Full VXC `text extract` module test with validation

set -euo pipefail

LOGFILE="log.txt"
TESTFILE="sample.txt"
EMPTYFILE="empty.txt"
> "$LOGFILE"
> "$EMPTYFILE"

trap "rm -f $TESTFILE $EMPTYFILE tmpout" EXIT

# Setup sample.txt
cat << EOF > "$TESTFILE"
The quick brown fox
jumps over the lazy dog.
THE QUICK BROWN FOX
Some random line.
Another quick line.
quickly does it.
Empty line follows:

Last line.
EOF

# Utility: Run command and compare output to expected
run_test() {
    local desc="$1"
    local cmd="$2"
    local expected="$3"
    local expected_exit=${4:-0}

    echo "Running: $desc" >> "$LOGFILE"
    echo "\$ $cmd" >> "$LOGFILE"

    if ! output=$(eval "$cmd" 2>tmpout); then
        actual_exit=$?
        if [[ "$actual_exit" -ne "$expected_exit" ]]; then
            echo "[FAIL] $desc"
            echo "Expected exit code: $expected_exit"
            echo "Got exit code: $actual_exit"
            cat tmpout >> "$LOGFILE"
            exit 1
        fi
    else
        actual_exit=0
    fi

    [[ -f tmpout ]] && cat tmpout >> "$LOGFILE"

    echo "$output" >> "$LOGFILE"
    echo "---" >> "$LOGFILE"

    if [[ "$output" != "$expected" ]]; then
        echo "[FAIL] $desc"
        echo "Expected: [$expected]"
        echo "Got:      [$output]"
        exit 1
    else
        echo "[PASS] $desc"
    fi
}

# Tests

run_test "Context around 'random'" \
    "vxc text extract $TESTFILE -contains 'random' -context 1" \
$'Some random line.\nTHE QUICK BROWN FOX\nSome random line.\nAnother quick line.'

run_test "Count lines with 'random'" \
    "vxc text extract $TESTFILE -contains 'random' -count" \
"1"

run_test "Count lines with 'quick'" \
    "vxc text extract $TESTFILE -contains 'quick' -count" \
"3"

run_test "Numbered match lines for 'quick'" \
    "vxc text extract $TESTFILE -contains 'quick' -number" \
$'1: The quick brown fox\n5: Another quick line.\n6: quickly does it.'

run_test "Unique, ignore-case match for 'quick'" \
    "vxc text extract $TESTFILE -match 'quick' -ignore-case -trim -unique" \
$'The quick brown fox\nTHE QUICK BROWN FOX\nAnother quick line.\nquickly does it.'

run_test "Invert match 'quick'" \
    "vxc text extract $TESTFILE -contains 'quick' -invert" \
$'jumps over the lazy dog.\nSome random line.\nEmpty line follows:\n\nLast line.'

run_test "Match 'fox' in lines 1..5" \
    "vxc text extract $TESTFILE -lines 1..5 -match 'fox'" \
"The quick brown fox"

run_test "Lines before 'ZZZ'" \
    "vxc text extract $TESTFILE -before 'ZZZ'" \
$'The quick brown fox\njumps over the lazy dog.\nTHE QUICK BROWN FOX\nSome random line.\nAnother quick line.\nquickly does it.\nEmpty line follows:\n\nLast line.'

run_test "Lines after 'me'" \
    "vxc text extract $TESTFILE -after 'me'" \
$'Another quick line.\nquickly does it.\nEmpty line follows:\n\nLast line.'

run_test "Lines after nonexistent pattern" \
    "vxc text extract $TESTFILE -after 'nonexistent'" \
""

run_test "Empty file with -head" \
    "vxc text extract $EMPTYFILE -head 3" \
""

run_test "Invalid regex pattern" \
    "vxc text extract $TESTFILE -match '[abc'" \
"" \
1

echo "All tests passed. Log saved to $LOGFILE"
