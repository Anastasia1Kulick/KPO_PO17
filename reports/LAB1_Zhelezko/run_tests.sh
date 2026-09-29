
set -e

cd "$(dirname "$0")"

g++ -std=c++17 -Wall -o /tmp/lab1_tests test_main.cpp

/tmp/lab1_tests
