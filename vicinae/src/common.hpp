#pragma once
// Include as few files as possible, this header is included in many places

class FocusNotifier;

template <class... Ts> struct overloads : Ts... {
  using Ts::operator()...;
};

template <typename T> struct PaginatedResponse {
  int totalCount = 0;
  int currentPage = 0;
  int totalPages = 0;
  std::vector<T> data;
};

template <typename T> struct Scored {
  T data;
  int score = 0;

  bool operator<(const Scored<T> &rhs) const { return score < rhs.score; }
  bool operator>(const Scored<T> &rhs) const { return score > rhs.score; }
  bool operator==(const Scored<T> &rhs) const { return score == rhs.score; }
};

class NonCopyable {
public:
  NonCopyable(const NonCopyable &) = delete;
  NonCopyable &operator=(const NonCopyable &) = delete;
  NonCopyable() = default;
};

template <class T> using OptionalRef = std::optional<std::reference_wrapper<T>>;

struct JsonFormItemWidget : public QWidget {
  virtual QJsonValue asJsonValue() const = 0;
  virtual void setValueAsJson(const QJsonValue &value) = 0;
  virtual FocusNotifier *focusNotifier() const = 0;

  JsonFormItemWidget(QWidget *parent = nullptr) : QWidget(parent) {}
};

struct LaunchProps {
  QString query;
  std::vector<std::pair<QString, QString>> arguments;
};

class CommandContext;

enum CommandMode { CommandModeInvalid, CommandModeView, CommandModeNoView, CommandModeMenuBar };
enum CommandType { CommandTypeBuiltin, CommandTypeExtension };

/**
 * Represents an entrypoint.
 * An entrypoint is always scoped under a provider.
 * extension = provider, command = entrypoint inside the extension
 */
struct EntrypointId {
  std::string provider;
  std::string entrypoint;

  operator std::string() const { return std::format("{}:{}", provider, entrypoint); }

  static EntrypointId fromSerialized(std::string_view s) {
    auto pos = s.find(":");
    if (pos == std::string::npos) return {};
    return {std::string{s.substr(0, pos)}, std::string{s.substr(pos + 1)}};
  }

  bool operator==(const EntrypointId &rhs) const {
    return provider == rhs.provider && entrypoint == rhs.entrypoint;
  }
};

namespace std {
template <> class hash<EntrypointId> {
public:
  std::uint64_t operator()(const EntrypointId &id) const { return std::hash<std::string>{}(id); }
};
}; // namespace std

struct QObjectDeleter {
  void operator()(QObject *obj) { obj->deleteLater(); }
};

template <typename T = QObject> using QObjectUniquePtr = std::unique_ptr<T, QObjectDeleter>;
template <typename T> using UniqueFutureWatcher = QObjectUniquePtr<QFutureWatcher<T>>;

class AbstractArgumentProvider {
  virtual std::vector<std::pair<QString, QString>> args() const = 0;
};

class NavigationController;
class CommandController;
class ServiceRegistry;
class OverlayController;
class SettingsController;

struct ApplicationContext {
  std::unique_ptr<NavigationController> navigation;
  std::unique_ptr<OverlayController> overlay;
  ServiceRegistry *services;
  std::unique_ptr<SettingsController> settings;
};
