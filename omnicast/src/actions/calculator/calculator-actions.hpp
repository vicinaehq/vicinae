#include "calculator-history-command.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/calculator-list-item-widget.hpp"
#include "ui/toast.hpp"
#include "ui/ui-controller.hpp"
#include <base-view.hpp>
#include <qnamespace.h>

class CopyCalculatorAnswerAction : public AbstractAction {
  AbstractCalculatorBackend::CalculatorResult m_item;
  bool m_addToHistory = true;

  void execute() override {
    auto ui = ServiceRegistry::instance()->UI();
    auto calculator = ServiceRegistry::instance()->calculatorService();
    auto clip = ServiceRegistry::instance()->clipman();

    if (m_addToHistory) { calculator->addRecord(m_item); }

    ui->popToRoot();
    ui->closeWindow();

    if (clip->copyText(m_item.answer)) {
      ui->setToast("Answer copied to clipboard");
    } else {
      ui->setToast("Failed to copy answer", ToastPriority::Danger);
    }
  }

public:
  CopyCalculatorAnswerAction(const AbstractCalculatorBackend::CalculatorResult &item,
                             bool addToHistory = true)
      : AbstractAction("Copy Result", BuiltinOmniIconUrl("copy-clipboard")), m_item(item),
        m_addToHistory(addToHistory) {}
};

class CopyCalculatorQuestionAndAnswerAction : public AbstractAction {
  CalculatorItem m_item;
  bool m_addToHistory = true;

  void execute() override {
    auto ui = ServiceRegistry::instance()->UI();
    auto calculator = ServiceRegistry::instance()->calculatorDb();
    auto clip = ServiceRegistry::instance()->clipman();
    auto result = QString("%1 = %2").arg(m_item.expression).arg(m_item.result);

    if (m_addToHistory) { calculator->insertComputation(m_item.expression, m_item.result); }

    ui->popToRoot();
    ui->closeWindow();

    if (clip->copyText(result)) {
      ui->setToast("Copied to clipboard");
    } else {
      ui->setToast("Failed to copy answer", ToastPriority::Danger);
    }
  }

public:
  CopyCalculatorQuestionAndAnswerAction(const CalculatorItem &item, bool addToHistory = true)
      : AbstractAction("Copy Question And Answer", BuiltinOmniIconUrl("copy-clipboard")), m_item(item),
        m_addToHistory(addToHistory) {}
};

class PutCalculatorQuestionAndAnswerInSearchBar : public AbstractAction {
  CalculatorItem m_item;

  void execute() override {
    auto ui = ServiceRegistry::instance()->UI();
    auto result = QString("%1 = %2").arg(m_item.expression).arg(m_item.result);

    ui->topView()->setSearchText(result);
  }

public:
  PutCalculatorQuestionAndAnswerInSearchBar(const CalculatorItem &item)
      : AbstractAction("Put answer in search bar", BuiltinOmniIconUrl("text")), m_item(item) {}
};

class OpenCalculatorHistoryAction : public AbstractAction {
  void execute() override {
    auto ui = ServiceRegistry::instance()->UI();

    ui->pushView(new CalculatorHistoryView);
  }

public:
  OpenCalculatorHistoryAction()
      : AbstractAction("Open Calculator History", BuiltinOmniIconUrl("calculator")) {}
};

class PutCalculatorAnswerInSearchBar : public AbstractAction {
  CalculatorItem m_item;

  void execute() override {
    auto ui = ServiceRegistry::instance()->UI();

    ui->topView()->setSearchText(m_item.result);
  }

public:
  PutCalculatorAnswerInSearchBar(const CalculatorItem &item)
      : AbstractAction("Put answer in search bar", BuiltinOmniIconUrl("text")), m_item(item) {}
};
