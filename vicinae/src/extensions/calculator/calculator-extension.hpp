#pragma once
#include "calculator-history-command.hpp"
#include "command-database.hpp"
#include "omni-icon.hpp"
#include "preference.hpp"

static const std::vector<Preference::DropdownData::Option> backendOptions = {
    {"libqalculate", "libqalculate"},
};

static const std::vector<Preference::DropdownData::Option> refreshRatesOptions = {
    {"Every hour", "hourly"},
    {"Every day", "daily"},
    {"Every week", "weekly"},
    {"Every month", "monthly"},
};

class CalculatorHistoryCommand : public BuiltinViewCommand<CalculatorHistoryView> {
  QString id() const override { return "history"; }
  QString name() const override { return "Calculator history"; }
  OmniIconUrl iconUrl() const override {
    return BuiltinOmniIconUrl("plus-minus-divide-multiply").setBackgroundTint(SemanticColor::Red);
  }
};

class CalculatorExtension : public BuiltinCommandRepository {
public:
  QString id() const override { return "calculator"; }
  QString name() const override { return "Calculator"; }
  QString description() const override { return "Do maths, convert units or search past calculations..."; }
  OmniIconUrl iconUrl() const override {
    return BuiltinOmniIconUrl("plus-minus-divide-multiply").setBackgroundTint(SemanticColor::Red);
  }

  CalculatorExtension() { registerCommand<CalculatorHistoryCommand>(); }

  std::vector<Preference> preferences() const override {
    auto backendPref = Preference::makeDropdown("backend", backendOptions);

    backendPref.setTitle("Calculator Backend");
    backendPref.setDescription("Which backend to use to perform calculations");
    backendPref.setDefaultValue("libqalculate");

    auto refreshRates = Preference::makeDropdown("rate-refresh-interval", refreshRatesOptions);

    refreshRates.setTitle("Refresh exchange rates");
    refreshRates.setDescription("How often the exchange rates should be refreshed. This assumes the selected "
                                "backend supports currency conversions.");
    refreshRates.setDefaultValue("hourly");

    auto updatePast = Preference::makeCheckbox("update-past");

    updatePast.setDefaultValue(true);
    updatePast.setTitle("Update past calculations");
    updatePast.setDescription("Update past calculations when the exchange rates are refreshed. This may "
                              "introduce additional latency when the rates get refreshed.");

    return {backendPref, refreshRates, updatePast};
  }

  void preferenceValuesChanged(const QJsonObject &value) const override {
    // TODO: parse object
    // TODO: get calculator service and update stuff
  }
};
