#include "services/calculator-service/qalculate/qalculate-backend.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <array>
#include <regex>
#include <qlocale.h>

namespace {

using Catch::Matchers::Equals;

QalculateBackend makeBackend() {
  // libqalculate formats results according to LC_NUMERIC
  setenv("LC_ALL", "C", 1);
  QalculateBackend backend;
  REQUIRE(backend.start());
  return backend;
}

void assertComputationResult(const QString &question, const QString &expected) {
  auto r = makeBackend().compute(question, {});
  REQUIRE(r);
  REQUIRE_THAT(r->answer.text.toStdString(), Equals(expected.toStdString()));
}

enum class DatetimeFormat {
  Local,      // "2026-06-17T20:52:38" (no timezone)
  UtcWithZ,   // "2026-06-17T18:52:38Z" (Z suffix)
  WithOffset, // "2026-06-17T14:52:38-04:00" (timezone offset)
};

struct DatetimeFormatSpec {
  std::regex pattern;
};

const std::array<DatetimeFormatSpec, 3> DATETIME_FORMATS{{
    {std::regex(R"(\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2})")},               // Local
    {std::regex(R"(\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z)")},              // UtcWithZ
    {std::regex(R"(\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}[+-]\d{2}:\d{2})")} // WithOffset
}};

bool isValidIsoDatetime(const std::string &str, DatetimeFormat format) {
  std::string_view trimmed = str;
  if (trimmed.size() >= 2 && trimmed.front() == '"' && trimmed.back() == '"') {
    trimmed = trimmed.substr(1, trimmed.length() - 2);
  }

  const auto &spec = DATETIME_FORMATS[static_cast<std::size_t>(format)];
  return std::regex_match(trimmed.begin(), trimmed.end(), spec.pattern);
}

void assertIsValidDatetime(const QString &question, DatetimeFormat format) {
  auto backend = makeBackend();
  auto r = backend.compute(question, {});
  REQUIRE(r);
  REQUIRE(isValidIsoDatetime(r->answer.text.toStdString(), format));
}

} // namespace

TEST_CASE("computes full expressions") {
  auto backend = makeBackend();

  auto result = backend.compute("2 + 2", {});

  REQUIRE(result);
  REQUIRE(result->type == AbstractCalculatorBackend::NORMAL);
  REQUIRE(result->answer.text == "4");
}

TEST_CASE("supports basic unit conversion") {
  assertComputationResult("100 g to kg", "0.1\u202Fkg");
  assertComputationResult("ns to us", "0.001\u202Fμs");
  assertComputationResult("ns to us", "0.001\u202Fμs");
  assertComputationResult("day in hrs", "24\u202Fh");
  assertComputationResult("day in min", "1440\u202Fmin");
  assertComputationResult("100mm to m", "0.1\u202Fm");
}

TEST_CASE("handles datetime operations and conversions") {
  assertIsValidDatetime("now", DatetimeFormat::Local);
  assertIsValidDatetime("now + 1d", DatetimeFormat::Local);
  assertIsValidDatetime("now to utc", DatetimeFormat::UtcWithZ);
  assertIsValidDatetime("now + 1   week to utc", DatetimeFormat::UtcWithZ);
  assertIsValidDatetime("now in havana", DatetimeFormat::WithOffset);
}

TEST_CASE("resolves timezones by city, casing and custom offset") {
  assertIsValidDatetime("now in buenos aires", DatetimeFormat::WithOffset);
  assertIsValidDatetime("now in NEW YORK", DatetimeFormat::WithOffset);
  assertIsValidDatetime("now in new_york", DatetimeFormat::WithOffset);
  assertIsValidDatetime("now to +05:30", DatetimeFormat::WithOffset);
  assertIsValidDatetime("now to -08:00", DatetimeFormat::WithOffset);
}

TEST_CASE("strips trailing operators before computing") {
  auto backend = makeBackend();

  auto result = backend.compute("2 +", {});

  REQUIRE(result);
  REQUIRE(result->answer.text == "2");
}

TEST_CASE("rewrites infix in to to for unit conversion") {
  auto backend = makeBackend();

  auto result = backend.compute("100in in m", {});

  REQUIRE(result);
  REQUIRE(result->type == AbstractCalculatorBackend::CONVERSION);
  REQUIRE(result->answer.text.contains("2.54"));
  REQUIRE(result->answer.unit);
  REQUIRE_FALSE(result->answer.unit->displayName.isEmpty());
}

TEST_CASE("normalizes storage unit shorthand") {
  auto backend = makeBackend();

  auto decimal = backend.compute("1 gb to mb", {});
  auto binary = backend.compute("1 gib to mib", {});

  REQUIRE(decimal);
  REQUIRE(decimal->type == AbstractCalculatorBackend::CONVERSION);
  REQUIRE(decimal->answer.text.contains("1000"));
  REQUIRE(decimal->answer.unit);
  REQUIRE_FALSE(decimal->answer.unit->displayName.isEmpty());

  REQUIRE(binary);
  REQUIRE(binary->type == AbstractCalculatorBackend::CONVERSION);
  REQUIRE(binary->answer.text.contains("1024"));
  REQUIRE(binary->answer.unit);
  REQUIRE_FALSE(binary->answer.unit->displayName.isEmpty());
}

TEST_CASE("supports currency conversions") {
  auto backend = makeBackend();
  auto result = backend.compute("1 USD to USD", {});

  REQUIRE(result);
  REQUIRE(result->type == AbstractCalculatorBackend::CONVERSION);
  REQUIRE(result->answer.text.contains("1"));
  REQUIRE(result->answer.unit);
  REQUIRE_FALSE(result->answer.unit->displayName.isEmpty());
}

TEST_CASE("basic currency support") {
  auto backend = makeBackend();
  const auto assertCurrency = [&](auto &&cur) {
    auto result = backend.compute(QString("1 %1 to %1").arg(cur), {});
    REQUIRE(result);
    REQUIRE(result->type == AbstractCalculatorBackend::CONVERSION);
    REQUIRE(result->answer.text.contains("1"));
    REQUIRE(result->answer.unit);
    REQUIRE_FALSE(result->answer.unit->displayName.isEmpty());
  };

  const auto currencies = {"AUD", "USD", "YEN", "EUR", "GBP"};

  for (auto currency : currencies) {
    assertCurrency(currency);
  }
}
