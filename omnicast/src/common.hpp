#pragma once
#include "argument.hpp"
#include "omni-icon.hpp"
#include "preference.hpp"
#include "theme.hpp"
#include "ui/focus-notifier.hpp"
#include <QHBoxLayout>
#include <QString>
#include <cmath>
#include <functional>
#include <optional>
#include <qboxlayout.h>
#include <qevent.h>
#include <qframe.h>
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

class HDivider : public QFrame {
  QColor _color;
  size_t _height;

  void paintEvent(QPaintEvent *event) override {
    QPainter painter(this);
    auto margins = contentsMargins();
    auto &theme = ThemeService::instance().theme();

    painter.setBrush(QBrush(theme.colors.border));
    painter.setPen(theme.colors.border);
    painter.drawRect(0, margins.top(), width(), _height);
  }

public:
  void setHeight(int height) {
    _height = height;
    setFixedHeight(height);
    updateGeometry();
  }

  void setColor(QColor color) {
    _color = color;
    update();
  }

  HDivider(QWidget *parent = nullptr) : _height(1), _color("#222222") { setFixedHeight(_height); }
};

class VDivider : public QFrame {
  QColor _color;
  size_t _width;

  void paintEvent(QPaintEvent *event) override {
    QPainter painter(this);
    auto margins = contentsMargins();
    auto &theme = ThemeService::instance().theme();

    painter.setBrush(QBrush(theme.colors.border));
    painter.setPen(theme.colors.border);
    painter.drawRect(0, margins.top(), _width, height() - margins.top() - margins.bottom());
  }

public:
  void setWidth(int width) {
    _width = width;
    setFixedWidth(width);
    updateGeometry();
  }

  void setColor(QColor color) {
    _color = color;
    update();
  }

  VDivider(QWidget *parent = nullptr) : QFrame(parent), _width(1), _color("#222222") {
    setFixedWidth(_width);
  }
};

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
  OmniIconUrl iconUrl;
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
  virtual OmniIconUrl iconUrl() const = 0;
  virtual CommandType type() const = 0;
  virtual CommandMode mode() const = 0;
  virtual std::vector<Preference> preferences() const { return {}; }
  virtual std::vector<CommandArgument> arguments() const { return {}; }
  virtual std::vector<QString> keywords() const { return {}; }
  virtual QString repositoryName() const { return ""; }
  virtual bool isFallback() const { return false; }
  virtual void preferenceValuesChanged(const QJsonObject &value) const {}

  virtual QString extensionId() const = 0;
  virtual QString commandId() const = 0;

  bool isView() const { return mode() == CommandModeView; }
  bool isNoView() const { return mode() == CommandModeNoView; }

  virtual CommandContext *createContext(const std::shared_ptr<AbstractCmd> &command) const { return nullptr; }
};

class AbstractCommandRepository {
public:
  virtual QString id() const = 0;
  virtual QString name() const = 0;
  virtual QString description() const { return ""; }
  virtual std::vector<std::shared_ptr<AbstractCmd>> commands() const = 0;
  virtual OmniIconUrl iconUrl() const = 0;
  virtual std::vector<Preference> preferences() const { return {}; }
  virtual QWidget *settingsDetail() const { return new QWidget; }
  virtual void preferenceValuesChanged(const QJsonObject &value) const {}
};

struct QObjectDeleter {
  void operator()(QObject *obj) { obj->deleteLater(); }
};

template <typename T = QObject> using QObjectUniquePtr = std::unique_ptr<T, QObjectDeleter>;

class AbstractArgumentProvider {
  virtual std::vector<std::pair<QString, QString>> args() const = 0;
};
