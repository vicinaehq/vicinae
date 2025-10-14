#pragma once
#include "argument.hpp"
#include "ui/image/url.hpp"
#include "preference.hpp"
#include "ui/focus-notifier.hpp"
#include "vicinae.hpp"
#include "ui/divider/divider.hpp"
#include <QHBoxLayout>
#include <QString>
#include <functional>
#include <optional>
#include <qboxlayout.h>
#include <qevent.h>
#include <qframe.h>
#include <qfuturewatcher.h>
#include <qicon.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpainter.h>
#include <qprocess.h>
#include <qstack.h>
#include <qwidget.h>
#include <qwindowdefs.h>

template <class... Ts> struct overloads : Ts... {
  using Ts::operator()...;
};

template <typename T> struct PaginatedResponse {
  int totalCount;
  int currentPage;
  int totalPages;
  std::vector<T> data;
};

class NonCopyable {
public:
  NonCopyable(const NonCopyable &) = delete;
  NonCopyable &operator=(const NonCopyable &) = delete;
  NonCopyable() {}
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

struct NavigationStatus {
  QString title;
  ImageURL iconUrl;
};

struct LaunchCommandOptions {
  QString searchQuery;
  std::optional<NavigationStatus> navigation;
};

struct PushViewOptions {
  QString searchQuery;
  std::optional<NavigationStatus> navigation;
};

class AppWindow;
class CommandContext;

enum CommandMode { CommandModeInvalid, CommandModeView, CommandModeNoView, CommandModeMenuBar };
enum CommandType { CommandTypeBuiltin, CommandTypeExtension };

class AbstractCmd {
public:
  virtual QString uniqueId() const = 0;
  virtual QString name() const = 0;
  virtual QString description() const = 0;
  virtual ImageURL iconUrl() const = 0;
  virtual CommandType type() const = 0;
  virtual CommandMode mode() const = 0;
  virtual QString author() const = 0;
  virtual std::vector<Preference> preferences() const { return {}; }
  virtual std::vector<CommandArgument> arguments() const { return {}; }
  virtual std::vector<QString> keywords() const { return {}; }
  virtual QString repositoryDisplayName() const { return ""; }
  virtual QString repositoryName() const { return ""; }
  virtual bool isFallback() const { return false; }
  virtual void preferenceValuesChanged(const QJsonObject &value) const {}

  /**
   * Optional override of the navigation title that is to be shown when
   * the command is execute from the root search. This only applies to view commands.
   * The default is to use the name of the command.
   */
  virtual QString navigationTitle() const { return name(); }
  virtual ImageURL navigationIcon() const { return iconUrl(); }

  virtual bool isDefaultDisabled() const { return false; }

  QString deeplink() const {
    return QString("%1://extensions/%2/%3/%4")
        .arg(Omnicast::APP_SCHEME)
        .arg(author())
        .arg(repositoryName())
        .arg(commandId());
  }

  virtual QString extensionId() const = 0;
  virtual QString commandId() const = 0;

  bool isView() const { return mode() == CommandModeView; }
  bool isNoView() const { return mode() == CommandModeNoView; }

  virtual CommandContext *createContext(const std::shared_ptr<AbstractCmd> &command) const { return nullptr; }
};

class AbstractCommandRepository {
public:
  virtual QString id() const = 0;
  virtual QString displayName() const = 0;
  virtual QString name() const { return id(); }
  virtual QString description() const { return ""; }
  virtual QString author() const = 0;
  virtual std::vector<std::shared_ptr<AbstractCmd>> commands() const = 0;
  virtual ImageURL iconUrl() const = 0;
  virtual std::vector<Preference> preferences() const { return {}; }
  virtual QWidget *settingsDetail() const { return new QWidget; }

  /**
   * Triggered the first time the provider is registered
   */
  virtual void initialized(const QJsonObject &preferences) const {}

  virtual void preferenceValuesChanged(const QJsonObject &value) const {}
};

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
