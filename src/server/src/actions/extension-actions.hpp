#pragma once
#include <QCoreApplication>
#include <QFuture>
#include <expected>
#include <functional>
#include <qstring.h>
#include <string>
#include "ui/action-panel/action.hpp"
#include "ui/image/url.hpp"

class InstallExtensionAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(InstallExtensionAction)

public:
  using DownloadResult = std::expected<QByteArray, std::string>;
  using Downloader = std::function<QFuture<DownloadResult>(const ApplicationContext *ctx)>;

  InstallExtensionAction(const QString &installId, const ImageURL &icon, Downloader downloader)
      : m_installId(installId), m_icon(icon), m_downloader(std::move(downloader)) {}

private:
  QString m_installId;
  ImageURL m_icon;
  Downloader m_downloader;

  std::optional<ImageURL> icon() const override { return m_icon; }
  void execute(ApplicationContext *ctx) override;
  QString title() const override { return tr("Install extension"); }
};

class UninstallExtensionAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(UninstallExtensionAction)

  QString m_id;

  std::optional<ImageURL> icon() const override { return ImageURL::builtin(BuiltinIcon::ComputerChip); }
  void execute(ApplicationContext *ctx) override;
  QString title() const override { return tr("Uninstall Extension"); }

public:
  UninstallExtensionAction(const QString &id) : m_id(id) { setStyle(AbstractAction::Style::Danger); }
};
