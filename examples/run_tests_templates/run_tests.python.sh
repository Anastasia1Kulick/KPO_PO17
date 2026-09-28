#!/usr/bin/env bash
# Шаблон для Python. Скопируйте в tests/run_tests.sh своей лабы и адаптируйте.
#
# Ожидается:
#   src/...            -- ваш код
#   tests/test_*.py    -- ваши тесты (pytest сам их найдёт)
#   tests/requirements.txt -- необязательно, доп. зависимости для тестов

set -e

if [[ -f tests/requirements.txt ]]; then
  pip install --quiet -r tests/requirements.txt
fi

# чтобы тесты видели ваш код из src/ через import
export PYTHONPATH="src:${PYTHONPATH:-}"

python3 -m pytest tests/ -v
