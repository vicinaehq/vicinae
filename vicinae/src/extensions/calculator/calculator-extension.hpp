#pragma once
#include "history/calculator-history-view.hpp"
#include "services/calculator-service/calculator-service.hpp"
#include "command-database.hpp"
#include "ui/image/url.hpp"
#include "preference.hpp"
#include "service-registry.hpp"
#include "single-view-command-context.hpp"
#include "vicinae.hpp"

static const std::vector<Preference::DropdownData::Option> refreshRatesOptions = {
    {"Every hour", "hourly"},
    {"Every day", "daily"},
    {"Every week", "weekly"},
    {"Every month", "monthly"},
};

class CalculatorHistoryCommand : public BuiltinViewCommand<CalculatorHistoryView> {
  QString id() const override { return "history"; }
  QString name() const override { return "Calculator history"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("plus-minus-divide-multiply").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
};

class CalculatorExtension : public BuiltinCommandRepository {
public:
  QString id() const override { return "calculator"; }
  QString displayName() const override { return "Calculator"; }
  QString description() const override { return "Do maths, convert units or search past calculations..."; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("plus-minus-divide-multiply").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  CalculatorExtension() { registerCommand<CalculatorHistoryCommand>(); }

  std::vector<Preference> preferences() const override {
    auto calc = ServiceRegistry::instance()->calculatorService();
    std::vector<Preference::DropdownData::Option> backendOptions;

    for (const auto &backend : calc->backends()) {
      backendOptions.emplace_back(Preference::DropdownData::Option{backend->displayName(), backend->id()});
    }

    auto backendPref = Preference::makeDropdown("backend", backendOptions);

    backendPref.setTitle("Calculator Backend");
    backendPref.setDescription("Which backend to use to perform calculations");

    if (!backendOptions.empty()) { backendPref.setDefaultValue(backendOptions.front().value); }

    return {backendPref};
  }

  void preferenceValuesChanged(const QJsonObject &value) const override {
    auto calc = ServiceRegistry::instance()->calculatorService();
    QString backendId = value.value("backend").toString();

    // make sure we always have a backend running
    if (!calc->setBackend(backendId) && !calc->backend()) { calc->startFirstHealthy(); }
  }
};
