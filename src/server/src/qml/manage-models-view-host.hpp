#pragma once
#include "bridge-view.hpp"
#include "services/ai/ai-provider.hpp"
#include <QVariantList>

class ManageModelsModel;

namespace AI {
class Service;
};

class ManageModelsViewHost : public ViewHostBase {
  Q_OBJECT

  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)
  Q_PROPERTY(bool hasDetail READ hasDetail NOTIFY detailChanged)
  Q_PROPERTY(QString detailContent READ detailContent NOTIFY detailChanged)
  Q_PROPERTY(QUrl detailContentUrl READ detailContentUrl NOTIFY detailChanged)
  Q_PROPERTY(QVariantList detailMetadata READ detailMetadata NOTIFY detailChanged)

signals:
  void detailChanged();

public:
  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;

  QObject *listModel() const;
  bool hasDetail() const { return m_hasDetail; }
  QString detailContent() const { return m_detailContent; }
  QUrl detailContentUrl() const { return m_hasDescription ? QUrl(QStringLiteral("qrc:/Vicinae/MarkdownDetailContent.qml")) : QUrl(); }
  QVariantList detailMetadata() const { return m_detailMetadata; }

private:
  void loadDetail(const AI::ProviderModel &model);
  void clearDetail();
  void reload();

  ManageModelsModel *m_model = nullptr;
  AI::Service *m_aiService = nullptr;
  bool m_hasDetail = false;
  bool m_hasDescription = false;
  QString m_detailContent;
  QVariantList m_detailMetadata;
};
