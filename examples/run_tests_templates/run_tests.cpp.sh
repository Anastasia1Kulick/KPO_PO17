#!/usr/bin/env bash
# Шаблон для C++. Скопируйте в tests/run_tests.sh своей лабы и адаптируйте.
#
# Простой вариант без фреймворка: пишете tests/test_main.cpp с функцией main(),
# которая вызывает ваши функции из src/ и делает assert()/сравнение вручную,
# завершаясь ненулевым кодом при провале (например, через `return 1;`
# либо через assert() -- по умолчанию в release-сборке assert выключен,
# поэтому НЕ собирайте с -DNDEBUG, если полагаетесь на assert).
#
# Если хотите фреймворк -- на ubuntu-latest уже есть cmake/make, можно подключить
# Catch2 (header-only) или GoogleTest через FetchContent в CMakeLists.txt.

set -e

g++ -std=c++17 -Wall -o /tmp/lab_tests src/*.cpp tests/*.cpp
/tmp/lab_tests
