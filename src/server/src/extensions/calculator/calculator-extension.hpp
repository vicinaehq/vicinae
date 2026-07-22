#pragma once
#include <QCoreApplication>
#include "qml/calc-history-view-host.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "services/calculator-service/calculator-service.hpp"
#include "command-database.hpp"
#include "ui/image/url.hpp"
#include "preference.hpp"
#include "services/toast/toast-service.hpp"
#include "service-registry.hpp"
#include "single-view-command-context.hpp"
#include "vicinae.hpp"
#include <qfuturewatcher.h>

class CalculatorHistoryCommand : public BuiltinViewCommand<CalcHistoryViewHost> {
  Q_DECLARE_TR_FUNCTIONS(CalculatorHistoryCommand)
  QString id() const override { return "history"; }
  QString name() const override { return tr("Calculator history"); }
  QString description() const override {
    return tr("Browse past calculations. You need to copy the result of a calculation for it to be saved "
              "in history.");
  }
  std::vector<QString> keywords() const override { return {"Calculator history"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("plus-minus-divide-multiply").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
};

class CalculatorRefreshRatesCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(CalculatorRefreshRatesCommand)
  QString id() const override { return "refresh-rates"; }
  QString name() const override { return tr("Refresh Exchange Rates"); }
  QString description() const override {
    return tr("Refresh exchange rates used by the calculator to provide currency conversion features. Not "
              "all backends may support currency conversions or manually refreshing the rates.");
  }
  std::vector<QString> keywords() const override { return {"Refresh Exchange Rates"}; }

  ImageURL iconUrl() const override {
    return ImageURL::builtin("globe-01").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  void execute(CommandController &ctrl) const override {
    using Watcher = QFutureWatcher<std::optional<std::string>>;

    auto calc = ctrl.context()->services->calculatorService();
    auto toast = ctrl.context()->services->toastService();
    auto task = calc->backend()->refreshExchangeRates();
    auto watcher = new QFutureWatcher<AbstractCalculatorBackend::RefreshExchangeRatesResult>;

    if (!calc->backend()->supportsRefreshExchangeRates()) {
      return toast->failure(tr("%1 can't refresh rates").arg(calc->backend()->displayName()));
    }

    ctrl.context()->navigation->clearSearchText();
    toast->dynamic(tr("Refreshing rates..."));
    watcher->setFuture(task);

    QObject::connect(watcher, &Watcher::finished, [watcher, toast]() {
      watcher->deleteLater();
      if (!watcher->isFinished()) return;
      if (auto result = watcher->result(); !result) {
        return toast->failure(QString::fromStdString(result.error()));
      }
      return toast->success(tr("Rates successfully refreshed"));
    });
  }
};

class CalculatorExtension : public BuiltinCommandRepository {
  Q_DECLARE_TR_FUNCTIONS(CalculatorExtension)

public:
  QString id() const override { return "calculator"; }
  QString displayName() const override { return tr("Calculator"); }
  QString description() const override {
    return tr("Do maths, convert units or search past calculations...");
  }
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

    backendPref.setTitle(tr("Calculator Backend"));
    backendPref.setDescription(tr("Which backend to use to perform calculations"));

    auto refreshOnStartup = Preference::makeCheckbox("refreshRatesOnStartup");

    refreshOnStartup.setDefaultValue(true);
    refreshOnStartup.setTitle(tr("Refresh rates on startup"));
    refreshOnStartup.setDescription(
        tr("Whether exchange rates should be refreshed every time the vicinae server is started. If the "
           "current backend does not support it, this is ignored."));

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
