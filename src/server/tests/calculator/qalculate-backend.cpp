#include "services/calculator-service/qalculate/qalculate-backend.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include <catch2/catch_test_macros.hpp>
#include <qlocale.h>

namespace {

using ComputeMode = AbstractCalculatorBackend::ComputeMode;

QalculateBackend makeBackend() {
  QalculateBackend backend;
  REQUIRE(backend.start());
  return backend;
}

void assertComputationResult(const QString &question, const QString &expected) {
  auto r = makeBackend().compute(question, {.mode = ComputeMode::MixedSearch});
  REQUIRE(r);
  REQUIRE(r->answer.text == expected);
}

} // namespace

TEST_CASE("computes full expressions") {
  auto backend = makeBackend();

  auto result = backend.compute("2 + 2", {.mode = ComputeMode::Full});

  REQUIRE(result);
  REQUIRE(result->type == AbstractCalculatorBackend::NORMAL);
  REQUIRE(result->answer.text == "4");
}

TEST_CASE("supports basic unit conversion") {
  assertComputationResult("100 g to kg", "0.1 kg");
  assertComputationResult("ns to us", "0.001 μs");
  assertComputationResult("ns to us", "0.001 μs");
  assertComputationResult("day in hrs", "24 h");
  assertComputationResult("day in min", "1440 min");
  assertComputationResult("100mm to m", "0.1 m");
}

TEST_CASE("rejects non expressions in mixed search mode") {
  auto backend = makeBackend();

  REQUIRE_FALSE(backend.compute("hello", {.mode = ComputeMode::MixedSearch}));
  REQUIRE_FALSE(backend.compute("2", {.mode = ComputeMode::MixedSearch}));
  REQUIRE_FALSE(backend.compute("+ 2", {.mode = ComputeMode::MixedSearch}));
}

TEST_CASE("accepts expressions in mixed search mode") {
  auto backend = makeBackend();

  auto arithmetic = backend.compute("2 + 2", {.mode = ComputeMode::MixedSearch});
  auto functionCall = backend.compute("sqrt(16)", {.mode = ComputeMode::MixedSearch});

  REQUIRE(arithmetic);
  REQUIRE(arithmetic->answer.text == "4");
  REQUIRE(functionCall);
  REQUIRE(functionCall->answer.text == "4");
}

TEST_CASE("strips trailing operators before computing") {
  auto backend = makeBackend();

  auto result = backend.compute("2 +", {.mode = ComputeMode::Full});

  REQUIRE(result);
  REQUIRE(result->answer.text == "2");
}

TEST_CASE("rewrites infix in to to for unit conversion") {
  auto backend = makeBackend();

  auto result = backend.compute("100in in m", {.mode = ComputeMode::MixedSearch});

  REQUIRE(result);
  REQUIRE(result->type == AbstractCalculatorBackend::CONVERSION);
  REQUIRE(result->answer.text.contains("2.54"));
  REQUIRE(result->answer.unit);
  REQUIRE_FALSE(result->answer.unit->displayName.isEmpty());
}

TEST_CASE("normalizes storage unit shorthand") {
  auto backend = makeBackend();

  auto decimal = backend.compute("1 gb to mb", {.mode = ComputeMode::MixedSearch});
  auto binary = backend.compute("1 gib to mib", {.mode = ComputeMode::MixedSearch});

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

TEST_CASE("Standalone currency name should not trigger conversion in mixed search mode") {
  auto r = makeBackend().compute("USD", {.mode = ComputeMode::MixedSearch});
  REQUIRE_FALSE(r);
}

TEST_CASE("Standalone currency name should trigger conversion in full mode") {
  auto r = makeBackend().compute("USD", {.mode = ComputeMode::Full});
  REQUIRE(r);
}

const auto CONSTANTS = {"pi", "e"};

TEST_CASE("raw constants should not expand in mixed search mode") {
  auto calc = makeBackend();

  for (auto constant : CONSTANTS) {
    REQUIRE_FALSE(calc.compute(constant, {.mode = ComputeMode::MixedSearch}));
  }
}

TEST_CASE("PI constant should expand in full mode") {
  auto calc = makeBackend();

  for (auto constant : CONSTANTS) {
    REQUIRE(calc.compute(constant, {.mode = ComputeMode::Full}));
  }
}

TEST_CASE("supports currency conversions") {
  auto backend = makeBackend();
  auto result = backend.compute("1 USD to USD", {.mode = ComputeMode::MixedSearch});

  REQUIRE(result);
  REQUIRE(result->type == AbstractCalculatorBackend::CONVERSION);
  REQUIRE(result->answer.text.contains("1"));
  REQUIRE(result->answer.unit);
  REQUIRE_FALSE(result->answer.unit->displayName.isEmpty());
}

TEST_CASE("basic currency support") {
  auto backend = makeBackend();
  const auto assertCurrency = [&](auto &&cur) {
    auto result = backend.compute(QString("1 %1 to %1").arg(cur), {.mode = ComputeMode::MixedSearch});
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
