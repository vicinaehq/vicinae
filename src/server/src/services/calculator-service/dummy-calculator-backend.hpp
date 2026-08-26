#pragma once
#include "abstract-calculator-backend.hpp"
#include <qpromise.h>

class DummyCalculatorBackend : public AbstractCalculatorBackend {
public:
  QString id() const override { return "dummy"; }

  ComputeResult compute(const QString &question, const ComputeOptions &opts) override {
    return std::unexpected(CalculatorError("No calculator backend available"));
  }

  QFuture<ComputeResult> asyncCompute(const QString &question, const ComputeOptions &opts) override {
    QPromise<ComputeResult> promise;
    promise.addResult(std::unexpected(CalculatorError("No calculator backend available")));
    promise.finish();
    return promise.future();
  }

  bool isActivatable() const override { return true; }
};
