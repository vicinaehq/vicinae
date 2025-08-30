#pragma once
#include "calculator-history-command.hpp"
#include "command-database.hpp"
#include "../../ui/image/url.hpp"
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
    auto mapOption = [](auto &&backend) {
      return Preference::DropdownData::Option{backend->displayName(), backend->id()};
    };
    auto backendOptions =
        calc->backends() | std::views::transform(mapOption) | std::ranges::to<std::vector>();
    auto backendPref = Preference::makeDropdown("backend", backendOptions);

    backendPref.setTitle("Calculator Backend");
    backendPref.setDescription("Which backend to use to perform calculations");

    if (!backendOptions.empty()) { backendPref.setDefaultValue(backendOptions.front().value); }

    // TODO: maybe we will add these again if we decide to handle them properly.
    // They were non functional until now.

    /*
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
                                                      */

    return {backendPref};
  }

  void preferenceValuesChanged(const QJsonObject &value) const override {
    auto calc = ServiceRegistry::instance()->calculatorService();
    QString backendId = value.value("backend").toString();

    calc->setBackend(backendId);
  }
};
