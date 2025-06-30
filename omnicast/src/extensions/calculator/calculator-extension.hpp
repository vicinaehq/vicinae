#pragma once
#include "calculator-history-command.hpp"
#include "command-builder.hpp"
#include "common.hpp"
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

class CalculatorExtension : public AbstractCommandRepository {
public:
  QString id() const override { return "calculator"; }
  QString name() const override { return "Calculator"; }
  QString description() const override { return "Do maths, convert units or search past calculations..."; }
  OmniIconUrl iconUrl() const override {
    return BuiltinOmniIconUrl("plus-minus-divide-multiply").setBackgroundTint(ColorTint::Red);
  }

  std::vector<std::shared_ptr<AbstractCmd>> commands() const override {
    auto history = CommandBuilder("history")
                       .withName("Calculator History")
                       .withIcon(iconUrl())
                       .toSingleView<CalculatorHistoryView>();

    return {history};
  }

  std::vector<Preference> preferences() const override {
    auto backendPref = Preference::makeDropdown(backendOptions);

    backendPref.setName("backend");
    backendPref.setTitle("Calculator Backend");
    backendPref.setDescription("Which backend to use to perform calculations");
    backendPref.setDefaultValue("libqalculate");

    auto refreshRates = Preference::makeDropdown(refreshRatesOptions);

    refreshRates.setName("rate-refresh-interval");
    refreshRates.setTitle("Refresh exchange rates");
    refreshRates.setDescription("How often the exchange rates should be refreshed. This assumes the selected "
                                "backend supports currency conversions.");
    refreshRates.setDefaultValue("hourly");

    auto updatePast = Preference::makeCheckbox();

    updatePast.setName("update-past");
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
