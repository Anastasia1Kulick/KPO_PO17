#!/usr/bin/env bash
set -e

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OUT_BIN="/tmp/lab1_tests"

g++ -std=c++17 -Wall -Wextra -I "$ROOT_DIR/src" \
    -o "$OUT_BIN" \
    "$ROOT_DIR/tests/TestLab1.cpp"

"$OUT_BIN"