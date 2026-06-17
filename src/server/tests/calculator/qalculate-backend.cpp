#include "services/calculator-service/qalculate/qalculate-backend.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"

#include <catch2/catch_test_macros.hpp>

namespace {

using ComputeMode = AbstractCalculatorBackend::ComputeMode;

AbstractCalculatorBackend::ComputeResult compute(AbstractCalculatorBackend &backend, const QString &question,
                                                 ComputeMode mode = ComputeMode::MixedSearch) {
  return backend.compute(question, {.mode = mode});
}

QalculateBackend makeBackend() {
  QalculateBackend backend;
  AbstractCalculatorBackend &calculator = backend;
  REQUIRE(calculator.start());
  return backend;
}

} // namespace

TEST_CASE("Qalculate backend computes full expressions") {
  auto backend = makeBackend();

  auto result = compute(backend, "2 + 2", ComputeMode::Full);

  REQUIRE(result);
  REQUIRE(result->type == AbstractCalculatorBackend::NORMAL);
  REQUIRE(result->answer.text == "4");
}

TEST_CASE("Qalculate backend rejects non expressions in mixed search mode") {
  auto backend = makeBackend();

  REQUIRE_FALSE(compute(backend, "hello"));
  REQUIRE_FALSE(compute(backend, "2"));
  REQUIRE_FALSE(compute(backend, "+ 2"));
}

TEST_CASE("Qalculate backend accepts expressions in mixed search mode") {
  auto backend = makeBackend();

  auto arithmetic = compute(backend, "2 + 2");
  auto functionCall = compute(backend, "sqrt(16)");

  REQUIRE(arithmetic);
  REQUIRE(arithmetic->answer.text == "4");
  REQUIRE(functionCall);
  REQUIRE(functionCall->answer.text == "4");
}

TEST_CASE("Qalculate backend strips trailing operators before computing") {
  auto backend = makeBackend();

  auto result = compute(backend, "2 +", ComputeMode::Full);

  REQUIRE(result);
  REQUIRE(result->answer.text == "2");
}

TEST_CASE("Qalculate backend rewrites infix in to to for unit conversion") {
  auto backend = makeBackend();

  auto result = compute(backend, "100in in m");

  REQUIRE(result);
  REQUIRE(result->type == AbstractCalculatorBackend::CONVERSION);
  REQUIRE(result->answer.text.contains("2.54"));
  REQUIRE(result->answer.unit);
  REQUIRE_FALSE(result->answer.unit->displayName.isEmpty());
}

TEST_CASE("Qalculate backend normalizes storage unit shorthand") {
  auto backend = makeBackend();

  auto decimal = compute(backend, "1 gb to mb");
  auto binary = compute(backend, "1 gib to mib");

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
