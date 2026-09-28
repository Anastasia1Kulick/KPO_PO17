#!/usr/bin/env bash
#
# Запускает tests/run_tests.sh для каждой переданной папки лабораторной
# (reports/<Фамилия>/<Лаба>) и печатает Markdown-отчёт (для GITHUB_STEP_SUMMARY).
#
# Контракт со студентом:
#   - файл tests/run_tests.sh обязателен (если REQUIRE_TESTS=true);
#   - должен быть исполняемым сценарием (bash), не требующим интерактивного ввода;
#   - запускается с рабочей директорией = папка лабы (reports/<Фамилия>/<Лаба>);
#     то есть внутри скрипта пути пишутся как src/... и tests/...
#   - код возврата 0 = тесты прошли, любой другой = тесты не прошли;
#   - сеть на GitHub-раннере есть, поэтому `pip install`, `mvn`, `go get` и т.п.
#     внутри run_tests.sh работают, но замедляют прогон -- лучше зависеть
#     только от того, что уже есть на ubuntu-latest (python3, openjdk, maven,
#     gradle, g++/gcc, cmake, make).
#
# Пример для Python (pytest):
#   #!/usr/bin/env bash
#   set -e
#   pip install --quiet -r tests/requirements.txt 2>/dev/null || true
#   python3 -m pytest tests/ -v
#
# Пример для Java (Maven, pom.xml лежит в src/):
#   #!/usr/bin/env bash
#   set -e
#   cd src && mvn -q -B test
#
# Пример для C++ (сборка + свой тестовый бинарь):
#   #!/usr/bin/env bash
#   set -e
#   g++ -std=c++17 -o /tmp/run_tests src/*.cpp tests/*.cpp
#   /tmp/run_tests

set -uo pipefail

TIMEOUT_SECONDS="${TEST_TIMEOUT_SECONDS:-300}"
REQUIRE_TESTS="${REQUIRE_TESTS:-true}"
OVERALL_FAIL=0

echo "## Результаты автотестов"
echo

if [[ $# -eq 0 ]]; then
  echo "Изменённых папок лабораторных не найдено."
  exit 0
fi

for lab_dir in "$@"; do
  [[ -d "$lab_dir" ]] || continue
  echo "### \`$lab_dir\`"
  echo

  runner="$lab_dir/tests/run_tests.sh"

  if [[ ! -f "$runner" ]]; then
    if [[ "$REQUIRE_TESTS" == "true" ]]; then
      echo "❌ Не найден \`tests/run_tests.sh\`. Тесты обязательны для каждой лабораторной."
      OVERALL_FAIL=1
    else
      echo "⚠️ Тесты не предоставлены — проверка пропущена."
    fi
    echo
    continue
  fi

  chmod +x "$runner"
  output_file="$(mktemp)"

  if ( cd "$lab_dir" && timeout "${TIMEOUT_SECONDS}s" bash "tests/run_tests.sh" ) > "$output_file" 2>&1; then
    status="✅ Тесты прошли."
  else
    exit_code=$?
    if [[ $exit_code -eq 124 ]]; then
      status="❌ Превышено время ожидания (${TIMEOUT_SECONDS} сек) — возможен бесконечный цикл или зависшая программа."
    else
      status="❌ Тесты не прошли (код завершения: $exit_code)."
    fi
    OVERALL_FAIL=1
  fi

  echo "<details><summary>Вывод run_tests.sh</summary>"
  echo
  echo '```'
  tail -c 20000 "$output_file"
  echo '```'
  echo "</details>"
  echo
  echo "$status"
  echo
  rm -f "$output_file"
done

exit $OVERALL_FAIL
