#!/usr/bin/env bash
# Шаблон для Java + Maven. Скопируйте в tests/run_tests.sh своей лабы и адаптируйте.
#
# Ожидается, что src/ содержит полноценный Maven-проект:
#   src/pom.xml
#   src/src/main/java/...
#   src/src/test/java/...     -- ваши JUnit-тесты
#
# Если у вас Gradle вместо Maven, замените команду на:
#   cd src && ./gradlew test --console=plain
# (не забудьте закоммитить gradlew и gradle-wrapper.jar)

set -e

cd src
mvn -q -B test
