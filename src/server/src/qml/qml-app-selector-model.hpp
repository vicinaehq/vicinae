#pragma once
#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include <memory>

class AbstractApplication;
class AppService;

class QmlAppSelectorModel : public QObject {
  Q_OBJECT
  Q_PROPERTY(QVariantList items READ items NOTIFY itemsChanged)
  Q_PROPERTY(QVariantMap currentItem READ currentItem NOTIFY currentItemChanged)

public:
  explicit QmlAppSelectorModel(QObject *parent = nullptr);

  QVariantList items() const { return m_items; }
  QVariantMap currentItem() const { return m_currentItem; }

  Q_INVOKABLE void select(const QVariantMap &item);
  Q_INVOKABLE void selectById(const QString &id);

  void updateDefaultApp(const std::shared_ptr<AbstractApplication> &app);

signals:
  void itemsChanged();
  void currentItemChanged();

private:
  void buildItems();

  AppService *m_appDb = nullptr;
  QVariantList m_items;
  QVariantMap m_currentItem;
  QVariantMap m_defaultEntry;
};
