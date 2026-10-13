#pragma once
#include <QByteArray>
#include <QCoreApplication>
#include <QString>
#include <expected>
#include <filesystem>
#include <functional>
#include <string>
#include <variant>
#include "ui/action-panel/action.hpp"

/**
 * Prompts for a destination using the platform file chooser, then writes the payload there.
 * The provider only runs when the action is executed.
 */
class SaveAsAction : public AbstractAction {
  Q_DECLARE_TR_FUNCTIONS(SaveAsAction)

public:
  /// Raw bytes to write, or an existing file to copy.
  using Content = std::variant<QByteArray, std::filesystem::path>;

  struct Payload {
    std::string suggestedName;
    Content content;
  };

  /// The error is shown to the user as is.
  using Provider = std::function<std::expected<Payload, QString>()>;

  explicit SaveAsAction(Provider provider);

  void execute(ApplicationContext *ctx) override;

private:
  Provider m_provider;
};
