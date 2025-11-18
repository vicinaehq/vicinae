#pragma once
#include "history/calculator-history-view.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "services/calculator-service/calculator-service.hpp"
#include "command-database.hpp"
#include "ui/image/url.hpp"
#include "preference.hpp"
#include "services/toast/toast-service.hpp"
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
  QString description() const override {
    return "Browse past calculations. You need to copy the result of a calculation for it to be saved in "
           "history.";
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("plus-minus-divide-multiply").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
};

class CalculatorRefreshRatesCommand : public BuiltinCallbackCommand {
  QString id() const override { return "refresh-rates"; }
  QString name() const override { return "Refresh Exchange Rates"; }
  QString description() const override {
    return "Refresh exchange rates used by the calculator to provide curreny conversion features. Not all "
           "backends may support curreny conversions or manually refreshing the rates.";
  }

  ImageURL iconUrl() const override {
    return ImageURL::builtin("globe-01").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  void execute(CommandController *ctrl) const override {
    using Watcher = QFutureWatcher<std::optional<std::string>>;

    auto calc = ctrl->context()->services->calculatorService();
    auto toast = ctrl->context()->services->toastService();
    auto task = calc->backend()->refreshExchangeRates();
    auto watcher = new QFutureWatcher<AbstractCalculatorBackend::RefreshExchangeRatesResult>;

    if (!calc->backend()->supportsRefreshExchangeRates()) {
      return toast->failure(QString("%1 can't refresh rates").arg(calc->backend()->displayName()));
    }

    ctrl->context()->navigation->clearSearchText();
    toast->dynamic("Refreshing rates...");
    watcher->setFuture(task);

    QObject::connect(watcher, &Watcher::finished, [watcher, toast]() {
      watcher->deleteLater();
      if (!watcher->isFinished()) return;
      if (auto result = watcher->result(); !result) {
        return toast->failure(QString::fromStdString(result.error()));
      }
      return toast->success("Rates successfully refreshed");
    });
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

  CalculatorExtension() {
    registerCommand<CalculatorHistoryCommand>();
    registerCommand<CalculatorRefreshRatesCommand>();
  }

  std::vector<Preference> preferences() const override {
    auto calc = ServiceRegistry::instance()->calculatorService();
    std::vector<Preference::DropdownData::Option> backendOptions;

    for (const auto &backend : calc->backends()) {
      backendOptions.emplace_back(Preference::DropdownData::Option{backend->displayName(), backend->id()});
    }

    auto backendPref = Preference::makeDropdown("backend", backendOptions);

    backendPref.setTitle("Calculator Backend");
    backendPref.setDescription("Which backend to use to perform calculations");

    auto refreshOnStartup = Preference::makeCheckbox("refreshRatesOnStartup");

    refreshOnStartup.setDefaultValue(true);
    refreshOnStartup.setTitle("Refresh rates on startup");
    refreshOnStartup.setDescription(
        "Whether exchange rates should be refreshed every time the vicinae server is started. If the current "
        "backend does not support it, this is ignored.");

    if (!backendOptions.empty()) { backendPref.setDefaultValue(backendOptions.front().value); }

    return {backendPref, refreshOnStartup};
  }

  void initialized(const QJsonObject &value) const override {
    auto calc = ServiceRegistry::instance()->calculatorService();
    bool refreshOnStartup = value.value("refreshRatesOnStartup").toBool();

    if (refreshOnStartup) { calc->backend()->refreshExchangeRates(); }
  }

  void preferenceValuesChanged(const QJsonObject &value) const override {
    auto calc = ServiceRegistry::instance()->calculatorService();
    QString backendId = value.value("backend").toString();

    // make sure we always have a backend running
    if (!calc->setBackend(backendId) && !calc->backend()) { calc->startFirstHealthy(); }
  }
};
