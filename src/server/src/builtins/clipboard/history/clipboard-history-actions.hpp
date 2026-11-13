#pragma once
#include <QCoreApplication>
#include <memory>
#include <string>
#include <utility>
#include "services/app-service/app-service.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "actions/clipboard-actions.hpp"
#include "ui/views/edit-keywords-view-host.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/alert/alert.hpp"

#include "utils/text-transforms.hpp"
#include <functional>

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

class PasteClipboardSelectionAsPlainText : public PasteToFocusedWindowAction {
  Q_DECLARE_TR_FUNCTIONS(PasteClipboardSelectionAsPlainText)
  QString m_id;

  void execute(ApplicationContext *ctx) override {
    setConcealed();
    auto clipman = ctx->services->clipman();
    if (auto data = clipman->getMainOfferData(m_id); data.has_value()) {
      loadClipboardData(Clipboard::Text{.text = QString::fromUtf8(*data)});
    } else {
      loadClipboardData(Clipboard::SelectionRecordHandle(m_id));
    }
    PasteToFocusedWindowAction::execute(ctx);
  }

public:
  QString title() const override { return tr("Paste as Plain Text"); }
  PasteClipboardSelectionAsPlainText(const QString &id) : PasteToFocusedWindowAction(), m_id(id) {}
};

class TransformTextAndCopyAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(TransformTextAndCopyAction)
  QString m_id;
  std::function<QString(const QString &)> m_transform;

  void execute(ApplicationContext *ctx) override {
    auto clipman = ctx->services->clipman();
    auto toast = ctx->services->toastService();
    if (auto data = clipman->getMainOfferData(m_id); data.has_value()) {
      QString text = QString::fromUtf8(*data);
      QString transformed = m_transform(text);
      if (clipman->copyText(transformed, {.concealed = true})) {
        ctx->navigation->showHud(tr("Transformed and copied"), ImageURL::builtin(BuiltinIcon::CopyClipboard));
        return;
      }
    }
    toast->failure(tr("Failed to transform text"));
  }

public:
  TransformTextAndCopyAction(const QString &title, const QString &id,
                             std::function<QString(const QString &)> transform)
      : AbstractAction(title, BuiltinIcon::Text), m_id(id), m_transform(std::move(transform)) {}
};

class TextTransformSubmenuAction : public ListSubmenuAction {
  Q_DECLARE_TR_FUNCTIONS(TextTransformSubmenuAction)
  QString m_id;

public:
  std::unique_ptr<ActionPanelState> buildState(ApplicationContext *ctx) const override {
    auto panel = std::make_unique<ActionPanelState>();
    auto section = panel->createSection();

    section->addAction(
        new TransformTextAndCopyAction(tr("Copy UPPERCASE"), m_id, text_transforms::toUppercase));
    section->addAction(
        new TransformTextAndCopyAction(tr("Copy lowercase"), m_id, text_transforms::toLowercase));
    section->addAction(
        new TransformTextAndCopyAction(tr("Copy Title Case"), m_id, text_transforms::toTitleCase));
    section->addAction(
        new TransformTextAndCopyAction(tr("Copy Slug (kebab-case)"), m_id, text_transforms::toSlug));
    section->addAction(new TransformTextAndCopyAction(tr("Copy Trimmed"), m_id, text_transforms::trim));

    return panel;
  }

  TextTransformSubmenuAction(const QString &id)
      : ListSubmenuAction(tr("Text Transformations..."), BuiltinIcon::Text), m_id(id) {}
};

class CopyColorFormatAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(CopyColorFormatAction)
  QString m_formatted;

  void execute(ApplicationContext *ctx) override {
    auto clipman = ctx->services->clipman();
    if (clipman->copyText(m_formatted, {.concealed = true})) {
      ctx->navigation->showHud(tr("Copied: %1").arg(m_formatted), ImageURL::builtin(BuiltinIcon::EyeDropper));
      return;
    }
    ctx->services->toastService()->failure(tr("Failed to copy color"));
  }

public:
  CopyColorFormatAction(const QString &title, QString formatted, const QColor &color)
      : AbstractAction(title, ImageURL::builtin(BuiltinIcon::EyeDropper).setBackgroundTint(color)),
        m_formatted(std::move(formatted)) {}
};

class CopyClipboardSelection : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(CopyClipboardSelection)
  QString m_id;
  void execute(ApplicationContext *ctx) override {
    auto clipman = ctx->services->clipman();
    auto toast = ctx->services->toastService();
    if (clipman->copySelectionRecord(m_id, {.concealed = true})) {
      ctx->navigation->showHud(tr("Selection copied to clipboard"));
      return;
    }
    toast->failure(tr("Failed to copy to clipboard"));
  }

public:
  CopyClipboardSelection(const QString &id)
      : AbstractAction(tr("Copy to clipboard"), BuiltinIcon::CopyClipboard), m_id(id) {}
};

/**
 * Opens the selection's main offer with the given application.
 * The offer is only exported to a file when the action is executed.
 * Edits made to the exported file will update the selection in real time.
 */
class OpenClipboardSelectionAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(OpenClipboardSelectionAction)
  std::shared_ptr<AbstractApplication> m_app;
  QString m_id;
  std::string m_fileName;
  void execute(ApplicationContext *ctx) override {
    auto toast = ctx->services->toastService();
    auto path = ctx->services->clipman()->exportMainOffer(m_id, m_fileName);

    if (!path) {
      toast->failure(tr("Failed to read clipboard data"));
      return;
    }

    if (!ctx->services->appDb()->launch(*m_app, {QString::fromStdString(path->string())})) {
      toast->failure(QCoreApplication::translate("OpenAppAction", "Failed to start app"));
      return;
    }

    ctx->navigation->closeWindow();
  }

public:
  OpenClipboardSelectionAction(const std::shared_ptr<AbstractApplication> &app, const QString &title,
                               const QString &id, std::string fileName)
      : AbstractAction(title, app->iconUrl()), m_app(app), m_id(id), m_fileName(std::move(fileName)) {}
};

class OpenClipboardSelectionWithAction : public ListSubmenuAction {
  QString m_id;
  QString m_mime;
  std::string m_fileName;

public:
  std::unique_ptr<ActionPanelState> buildState(ApplicationContext *ctx) const override {
    auto panel = std::make_unique<ActionPanelState>();
    auto section = panel->createSection();

    for (const auto &opener : ctx->services->appDb()->findOpenersForMime(m_mime)) {
      section->addAction(new OpenClipboardSelectionAction(opener, opener->displayName(), m_id, m_fileName));
    }

    return panel;
  }

  OpenClipboardSelectionWithAction(const QString &id, const QString &mime, std::string fileName)
      : ListSubmenuAction(QCoreApplication::translate("OpenWithAction", "Open with..."),
                          BuiltinIcon::ArrowUp),
        m_id(id), m_mime(mime), m_fileName(std::move(fileName)) {}
};

class RemoveSelectionAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(RemoveSelectionAction)
  QString m_id;
  void execute(ApplicationContext *ctx) override {
    auto clipman = ctx->services->clipman();
    auto toast = ctx->services->toastService();
    if (clipman->removeSelection(m_id)) {
      toast->setToast(tr("Entry removed"));
    } else {
      toast->setToast(tr("Failed to remove entry"), ToastStyle::Danger);
    }
  }

public:
  RemoveSelectionAction(const QString &id)
      : AbstractAction(tr("Remove entry"), ImageURL::builtin(BuiltinIcon::Trash)), m_id(id) {
    setStyle(AbstractAction::Style::Danger);
  }
};

class PinClipboardAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(PinClipboardAction)
  QString m_id;
  bool m_value;
  void execute(ApplicationContext *ctx) override {
    if (ctx->services->clipman()->setPinned(m_id, m_value)) {
      ctx->services->toastService()->success(m_value ? tr("Selection pinned") : tr("Selection unpinned"));
    } else {
      ctx->services->toastService()->failure(tr("Failed to change pin status"));
    }
  }

public:
  PinClipboardAction(const QString &id, bool value)
      : AbstractAction(value ? tr("Pin") : tr("Unpin"), ImageURL::builtin(BuiltinIcon::Pin)), m_id(id),
        m_value(value) {}
};

class EditClipboardKeywordsAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(EditClipboardKeywordsAction)
  QString m_id;
  void execute(ApplicationContext *ctx) override {
    auto clipman = ctx->services->clipman();
    auto id = m_id;
    auto view =
        new EditKeywordsViewHost([clipman, id]() { return clipman->retrieveKeywords(id).value_or(""); },
                                 [clipman, id](const QString &kw) { return clipman->setKeywords(id, kw); },
                                 tr("Additional keywords that will be used to index this selection."));
    ctx->navigation->pushView(view);
    ctx->navigation->setNavigationTitle(title());
  }

public:
  QString title() const override { return tr("Edit keywords"); }
  std::optional<ImageURL> icon() const override { return ImageURL::builtin(BuiltinIcon::Text); }
  EditClipboardKeywordsAction(const QString &id) : m_id(id) {}
};

class RemoveAllSelectionsAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(RemoveAllSelectionsAction)
  void execute(ApplicationContext *ctx) override {
    auto alert = new CallbackAlertWidget();
    alert->setTitle(tr("Are you sure?"));
    alert->setMessage(tr("All your clipboard history will be lost forever"));
    alert->setConfirmText(tr("Delete all"), SemanticColor::Red);
    alert->setConfirmCallback([ctx]() {
      auto toast = ctx->services->toastService();
      auto clipman = ctx->services->clipman();
      if (clipman->removeAllSelections()) {
        toast->success(tr("All selections were removed"));
      } else {
        toast->failure(tr("Failed to remove all selections"));
      }
    });
    ctx->navigation->setDialog(alert);
  }

public:
  QString title() const override { return tr("Remove all"); }
  std::optional<ImageURL> icon() const override { return ImageURL::builtin(BuiltinIcon::Trash); }
  RemoveAllSelectionsAction() { setStyle(AbstractAction::Style::Danger); }
};
