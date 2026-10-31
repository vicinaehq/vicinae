#pragma once

#include <QObject>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>
#include "services/ai/tool-registry.hpp"

class RootItemManager;

class ToolSettingsModel : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(ToolSettingsModel)
  QML_UNCREATABLE("")
  Q_PROPERTY(QVariantList groups READ groups NOTIFY groupsChanged)
  Q_PROPERTY(bool enabled READ enabled NOTIFY stateChanged)
  Q_PROPERTY(bool aiEnabled READ aiEnabled NOTIFY stateChanged)
  Q_PROPERTY(QVariantList policyItems READ policyItems CONSTANT)

public:
  Q_INVOKABLE void setToolPolicy(const QString &providerId, const QString &toolId, const QString &policy);

signals:
  void groupsChanged();
  void stateChanged();

public:
  explicit ToolSettingsModel(QObject *parent = nullptr);

  QVariantList groups() const { return m_groups; }
  bool enabled() const;
  bool aiEnabled() const;
  QVariantList policyItems() const;

private:
  void rebuild();

  AI::ToolRegistry &m_registry;
  RootItemManager &m_rootItems;
  QVariantList m_groups;
};
