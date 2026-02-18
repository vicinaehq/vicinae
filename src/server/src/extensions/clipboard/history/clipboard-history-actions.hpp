#pragma once
#include "builtin_icon.hpp"
#include "clipboard-actions.hpp"
#include "qml/qml-edit-keywords-view-host.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-pannel/push-action.hpp"
#include "ui/alert/alert.hpp"

class PasteClipboardSelection : public PasteToFocusedWindowAction {
  QString m_id;
  void execute(ApplicationContext *ctx) override {
    setConcealed();
    loadClipboardData(Clipboard::SelectionRecordHandle(m_id));
    PasteToFocusedWindowAction::execute(ctx);
  }

public:
  PasteClipboardSelection(const QString &id) : PasteToFocusedWindowAction(), m_id(id) {}
};

class CopyClipboardSelection : public AbstractAction {
  QString m_id;
  void execute(ApplicationContext *ctx) override {
    auto clipman = ctx->services->clipman();
    auto toast = ctx->services->toastService();
    if (clipman->copySelectionRecord(m_id, {.concealed = true})) {
      ctx->navigation->showHud("Selection copied to clipboard");
      return;
    }
    toast->failure("Failed to copy to clipboard");
  }

public:
  CopyClipboardSelection(const QString &id)
      : AbstractAction("Copy to clipboard", BuiltinIcon::CopyClipboard), m_id(id) {}
};

class RemoveSelectionAction : public AbstractAction {
  QString m_id;
  void execute(ApplicationContext *ctx) override {
    auto clipman = ctx->services->clipman();
    auto toast = ctx->services->toastService();
    if (clipman->removeSelection(m_id)) {
      toast->setToast("Entry removed");
    } else {
      toast->setToast("Failed to remove entry", ToastStyle::Danger);
    }
  }

public:
  RemoveSelectionAction(const QString &id)
      : AbstractAction("Remove entry", ImageURL::builtin("trash")), m_id(id) {
    setStyle(AbstractAction::Style::Danger);
  }
};

class PinClipboardAction : public AbstractAction {
  QString m_id;
  bool m_value;
  void execute(ApplicationContext *ctx) override {
    QString action = m_value ? "pinned" : "unpinned";
    if (ctx->services->clipman()->setPinned(m_id, m_value)) {
      ctx->services->toastService()->success(QString("Selection %1").arg(action));
    } else {
      ctx->services->toastService()->failure("Failed to change pin status");
    }
  }

public:
  PinClipboardAction(const QString &id, bool value)
      : AbstractAction(value ? "Pin" : "Unpin", ImageURL::builtin("pin")), m_id(id), m_value(value) {}
};

class EditClipboardKeywordsAction : public AbstractAction {
  QString m_id;
  void execute(ApplicationContext *ctx) override {
    auto clipman = ctx->services->clipman();
    auto id = m_id;
    auto view = new QmlEditKeywordsViewHost(
        [clipman, id]() { return clipman->retrieveKeywords(id).value_or(""); },
        [clipman, id](const QString &kw) { return clipman->setKeywords(id, kw); },
        "Additional keywords that will be used to index this selection.");
    ctx->navigation->pushView(view);
    ctx->navigation->setNavigationTitle(title());
  }

public:
  QString title() const override { return "Edit keywords"; }
  std::optional<ImageURL> icon() const override { return ImageURL::builtin("text"); }
  EditClipboardKeywordsAction(const QString &id) : m_id(id) {}
};

class RemoveAllSelectionsAction : public AbstractAction {
  void execute(ApplicationContext *ctx) override {
    auto alert = new CallbackAlertWidget();
    alert->setTitle("Are you sure?");
    alert->setMessage("All your clipboard history will be lost forever");
    alert->setConfirmText("Delete all", SemanticColor::Red);
    alert->setConfirmCallback([ctx]() {
      auto toast = ctx->services->toastService();
      auto clipman = ctx->services->clipman();
      if (clipman->removeAllSelections()) {
        toast->success("All selections were removed");
      } else {
        toast->failure("Failed to remove all selections");
      }
    });
    ctx->navigation->setDialog(alert);
  }

public:
  QString title() const override { return "Remove all"; }
  std::optional<ImageURL> icon() const override { return ImageURL::builtin("trash"); }
  RemoveAllSelectionsAction() { setStyle(AbstractAction::Style::Danger); }
};
