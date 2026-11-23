#!/usr/bin/env bash
set -e

LAB_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SRC_DIR="$LAB_DIR/src"
TEST_DIR="$LAB_DIR/tests"
OUT_BIN="/tmp/lab2_tests"

g++ -std=c++17 -Wall -Wextra \
    -I "$SRC_DIR" \
    -o "$OUT_BIN" \
    "$TEST_DIR/TestLab2.cpp"

cd "$TEST_DIR"
"$OUT_BIN"