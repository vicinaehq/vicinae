#include <catch2/catch_test_macros.hpp>

#include "qml/dmenu-output.hpp"

TEST_CASE("dmenu formats selected entries") {
  QString const text = QStringLiteral("duplicate");

  CHECK(formatDMenuOutput(text, 0, false) == text);
  CHECK(formatDMenuOutput(text, 0, true) == QStringLiteral("0"));
  CHECK(formatDMenuOutput(text, 2, true) == QStringLiteral("2"));
}
