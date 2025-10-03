#pragma once
#include "ui/action-pannel/action.hpp"
#include "service-registry.hpp"
#include "services/files-service/file-service.hpp"
#include <filesystem>

/**
 * A wrapper action that automatically tracks file access for any file-related action.
 */
class FileActionWrapper : public AbstractAction {
public:
  FileActionWrapper(AbstractAction *action, const std::filesystem::path &path)
      : AbstractAction(action->title(), action->icon()), m_wrappedAction(action), m_path(path) {

    setPrimary(action->isPrimary());
    setAutoClose(action->autoClose());
  }

  void execute(ApplicationContext *ctx) override {
    m_wrappedAction->execute(ctx);

    // Track the file access
    auto files = ctx->services->fileService();
    files->saveAccess(m_path);
  }

  bool isBoundTo(const QKeyEvent *event) const { return m_wrappedAction->isBoundTo(event); }

  bool isPushView() const override { return m_wrappedAction->isPushView(); }

  QString title() const override { return m_wrappedAction->title(); }

  ImageURL icon() const override { return m_wrappedAction->icon(); }

private:
  std::unique_ptr<AbstractAction> m_wrappedAction;
  std::filesystem::path m_path;
};
