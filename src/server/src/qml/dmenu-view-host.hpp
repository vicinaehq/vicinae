#pragma once
#include "bridge-view.hpp"
#include "vicinae-ipc/ipc.hpp"
#include <qmimedatabase.h>

class DMenuModel;

class DMenuViewHost : public ViewHostBase {
  Q_OBJECT
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)
  Q_PROPERTY(bool hasDetail READ hasDetail NOTIFY detailChanged)
  Q_PROPERTY(QString detailName READ detailName NOTIFY detailChanged)
  Q_PROPERTY(QString detailPath READ detailPath NOTIFY detailChanged)
  Q_PROPERTY(QString detailMimeType READ detailMimeType NOTIFY detailChanged)
  Q_PROPERTY(QString detailImageSource READ detailImageSource NOTIFY detailChanged)
  Q_PROPERTY(QString detailTextContent READ detailTextContent NOTIFY detailChanged)

signals:
  void detailChanged();
  void selected(const QString &text);

public:
  explicit DMenuViewHost(ipc::DMenu::Request data);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;
  void beforePop() override;
  bool showBackButton() const override { return false; }

  QObject *listModel() const;
  bool hasDetail() const { return m_hasDetail; }
  QString detailName() const { return m_detailName; }
  QString detailPath() const { return m_detailPath; }
  QString detailMimeType() const { return m_detailMimeType; }
  QString detailImageSource() const { return m_detailImageSource; }
  QString detailTextContent() const { return m_detailTextContent; }

private:
  void loadDetail(std::string_view path);
  void clearDetail();

  ipc::DMenu::Request m_data;
  DMenuModel *m_model = nullptr;
  QMimeDatabase m_mimeDb;
  bool m_selected = false;

  bool m_hasDetail = false;
  QString m_detailName;
  QString m_detailPath;
  QString m_detailMimeType;
  QString m_detailImageSource;
  QString m_detailTextContent;
};
