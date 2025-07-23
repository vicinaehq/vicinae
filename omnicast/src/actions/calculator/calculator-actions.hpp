#pragma once
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/alert.hpp"
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
  AbstractCalculatorBackend::CalculatorResult m_item;
  bool m_addToHistory = true;

  void execute() override {
    auto ui = ServiceRegistry::instance()->UI();
    auto calculator = ServiceRegistry::instance()->calculatorService();
    auto clip = ServiceRegistry::instance()->clipman();
    auto result = QString("%1 = %2").arg(m_item.question).arg(m_item.answer);

    if (m_addToHistory) { calculator->addRecord(m_item); }

    ui->closeWindow();

    if (clip->copyText(result)) {
      ui->setToast("Copied to clipboard");
    } else {
      ui->setToast("Failed to copy answer", ToastPriority::Danger);
    }
  }

public:
  CopyCalculatorQuestionAndAnswerAction(const AbstractCalculatorBackend::CalculatorResult &item,
                                        bool addToHistory = true)
      : AbstractAction("Copy Question And Answer", BuiltinOmniIconUrl("copy-clipboard")), m_item(item),
        m_addToHistory(addToHistory) {}
};

class OpenCalculatorHistoryAction : public AbstractAction {
  void execute() override;

public:
  OpenCalculatorHistoryAction()
      : AbstractAction("Open Calculator History", BuiltinOmniIconUrl("calculator")) {}
};

class PutCalculatorAnswerInSearchBar : public AbstractAction {
  AbstractCalculatorBackend::CalculatorResult m_item;

  void execute() override {
    auto ui = ServiceRegistry::instance()->UI();

    ui->setSearchText(m_item.answer);
  }

public:
  QString title() const override { return "Put answer in search bar"; }
  OmniIconUrl icon() const override { return BuiltinOmniIconUrl("text"); }

  PutCalculatorAnswerInSearchBar(const AbstractCalculatorBackend::CalculatorResult &item) : m_item(item) {}
};

class PinCalculatorHistoryRecordAction : public AbstractAction {
  int m_id = -1;

public:
  void execute() override {
    auto ui = ServiceRegistry::instance()->UI();
    auto calculator = ServiceRegistry::instance()->calculatorService();

    calculator->pinRecord(m_id);
    ui->setToast("Entry pinned");
  }

  QString title() const override { return "Pin entry"; }
  OmniIconUrl icon() const override { return BuiltinOmniIconUrl("pin"); }

  PinCalculatorHistoryRecordAction(int id) : m_id(id) {}
};

class UnpinCalculatorHistoryRecordAction : public AbstractAction {
  int m_id = -1;

public:
  void execute() override {
    auto ui = ServiceRegistry::instance()->UI();
    auto calculator = ServiceRegistry::instance()->calculatorService();

    calculator->unpinRecord(m_id);
    ui->setToast("Entry unpinned");
  }

  QString title() const override { return "Unpin entry"; }
  OmniIconUrl icon() const override { return BuiltinOmniIconUrl("pin-disabled"); }

  UnpinCalculatorHistoryRecordAction(int id) : m_id(id) {}
};

class RemoveCalculatorHistoryRecordAction : public AbstractAction {
  int m_id = -1;

public:
  void execute() override {
    auto ui = ServiceRegistry::instance()->UI();

    if (ServiceRegistry::instance()->calculatorService()->removeRecord(m_id)) {
      ui->setToast("Entry removed");
    } else {
      ui->setToast("Failed to remove entry", ToastPriority::Danger);
    }
  }

  QString title() const override { return "Delete entry"; }
  OmniIconUrl icon() const override { return BuiltinOmniIconUrl("trash"); }

  RemoveCalculatorHistoryRecordAction(int id) : m_id(id) { setStyle(AbstractAction::Style::Danger); }
};

class RemoveAllCalculatorHistoryRecordsAction : public AbstractAction {
  class ConfirmAlert : public AlertWidget {
    void confirm() const override {
      auto ui = ServiceRegistry::instance()->UI();
      auto calculator = ServiceRegistry::instance()->calculatorService();

      if (calculator->removeAll()) {
        ui->setToast("All entries were deleted");
      } else {
        ui->setToast("Failed to delete all entries", ToastPriority::Danger);
      }
    }

    void canceled() const override {}

  public:
    ConfirmAlert() {
      setTitle("Are you sure?");
      setMessage("The current calculator history will be lost forever.");
      setConfirmText("Remove entries", SemanticColor::Red);
    }
  };

  void execute() override { ServiceRegistry::instance()->UI()->setAlert(new ConfirmAlert()); }

public:
  QString title() const override { return "Delete all entries"; }
  OmniIconUrl icon() const override { return BuiltinOmniIconUrl("trash"); }

  RemoveAllCalculatorHistoryRecordsAction() { setStyle(AbstractAction::Danger); }
};
