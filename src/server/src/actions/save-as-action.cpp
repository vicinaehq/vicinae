#include "actions/save-as-action.hpp"
#include <QDebug>
#include <QSaveFile>
#include <QtConcurrent/QtConcurrent>
#include <expected>
#include <optional>
#include <system_error>
#include <utility>
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/builtin-icon/builtin-icon.hpp"
#include "services/file-chooser/file-chooser-service.hpp"
#include "services/toast/toast-service.hpp"

namespace fs = std::filesystem;

static std::expected<void, QString> writeContent(const SaveAsAction::Content &content, const fs::path &dest) {
  if (const auto *source = std::get_if<fs::path>(&content)) {
    std::error_code ec;
    fs::copy_file(*source, dest, fs::copy_options::overwrite_existing, ec);
    if (ec) return std::unexpected(QString::fromStdString(ec.message()));
    return {};
  }

  const auto &data = std::get<QByteArray>(content);
  QSaveFile file(QString::fromStdString(dest.string()));

  if (!file.open(QIODevice::WriteOnly) || file.write(data) != data.size() || !file.commit()) {
    return std::unexpected(file.errorString());
  }

  return {};
}

SaveAsAction::SaveAsAction(Provider provider)
    : AbstractAction(tr("Save as…"), ImageURL::builtin(BuiltinIcon::SaveDocument)),
      m_provider(std::move(provider)) {}

void SaveAsAction::execute(ApplicationContext *ctx) {
  auto payload = m_provider();

  if (!payload) {
    qWarning() << "Save as: failed to load content:" << payload.error();
    ctx->services->toastService()->failure(payload.error());
    return;
  }

  auto *navigation = ctx->navigation.get();

  ctx->services->fileChooserService()->saveFile(
      {.suggestedName = std::move(payload->suggestedName)},
      [navigation, content = std::move(payload->content)](std::optional<fs::path> dest) {
        if (!dest) return;

        QtConcurrent::run([content, dest = *dest]() {
          auto result = writeContent(content, dest);
          if (!result) qWarning() << "Save as: failed to write" << dest.string() << ":" << result.error();
          return result.has_value();
        })
            .then(navigation, [navigation,
                               name = QString::fromStdString(dest->filename().string())](bool saved) {
              if (saved) {
                navigation->showHud(tr("Saved %1").arg(name), ImageURL::builtin(BuiltinIcon::SaveDocument));
              } else {
                navigation->showHud(tr("Failed to save %1").arg(name),
                                    ImageURL::builtin(BuiltinIcon::Warning).setFill(SemanticColor::Red));
              }
            });
      });
}
