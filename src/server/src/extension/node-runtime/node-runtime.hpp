#pragma once
#include <QObject>
#include <QString>
#include <filesystem>
#include <optional>

// Owns the node binary used to run the extension host: resolves an executable
// (VICINAE_NODE_BIN override -> managed runtime -> system node) and, on
// self-distributed builds, downloads/verifies/extracts the pinned runtime.
class NodeRuntime : public QObject {
  Q_OBJECT

public:
  explicit NodeRuntime(QObject *parent = nullptr);

  // Resolve a usable node executable. On managed builds, if the runtime isn't
  // installed yet this kicks off a one-time async download and returns nullopt;
  // `installed` fires once it becomes available.
  std::optional<std::filesystem::path> executable();

  bool provisioning() const { return m_downloadStarted; }

signals:
  void installed();
  void installFailed(const QString &error);

private:
  std::filesystem::path managedExecutable() const;
  void download();
  void install(const std::filesystem::path &archive);
  bool extract(const std::filesystem::path &archive, const std::filesystem::path &dest);
#ifndef Q_OS_WIN
  static std::filesystem::path tagged(const std::filesystem::path &node);
#endif

  bool m_downloadStarted = false;
};
