#pragma once
#include "completion-model.hpp"
#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include <memory>

class AbstractApplication;
class AppService;

class AppSelectorModel : public QObject {
  Q_OBJECT
  Q_PROPERTY(CompletionModel *model READ model CONSTANT)
  Q_PROPERTY(QVariantMap currentItem READ currentItem NOTIFY currentItemChanged)

public:
  explicit AppSelectorModel(QObject *parent = nullptr);

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
  CompletionModel m_model{this};
  QVariantMap m_currentItem;
  QVariantMap m_defaultEntry;
};
