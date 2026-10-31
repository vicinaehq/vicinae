#pragma once
#include <QtQml/qqmlregistration.h>
#include "ui/quick/completion-model.hpp"
#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include <memory>

class AbstractApplication;
class AppService;

struct AppSelectorOptions {
  bool openersOnly = false;
  bool includeDefaultBrowser = false;
  bool includeActions = false;
};

class AppSelectorModel : public QObject {
  Q_OBJECT
  QML_ANONYMOUS
  Q_PROPERTY(CompletionModel *model READ model CONSTANT)
  Q_PROPERTY(QVariantMap currentItem READ currentItem NOTIFY currentItemChanged)

public:
  using Options = AppSelectorOptions;

  static constexpr Options OPENER_OPTIONS{
      .openersOnly = true, .includeDefaultBrowser = true, .includeActions = true};

  explicit AppSelectorModel(QObject *parent = nullptr, const Options &options = {});

  CompletionModel *model() { return &m_model; }
  QVariantMap currentItem() const { return m_currentItem; }

  Q_INVOKABLE void select(const QVariantMap &item);
  Q_INVOKABLE void selectById(const QString &id);

  void updateDefaultApp(const std::shared_ptr<AbstractApplication> &app);

signals:
  void currentItemChanged();

private:
  void buildItems();

  AppService *m_appDb = nullptr;
  Options m_options;
  CompletionModel m_model{this};
  QVariantMap m_currentItem;
  QVariantMap m_defaultEntry;
};
