#!/usr/bin/env bash
set -e

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OUT_BIN="/tmp/lab2_tests"

g++ -std=c++17 -Wall -Wextra -I "$ROOT_DIR" \
    -o "$OUT_BIN" \
    "$ROOT_DIR/TestLab2.cpp"

"$OUT_BIN"