#pragma once
#include "raycast-store-model.hpp"
#include "section-list-model.hpp"
#include "bridge-view.hpp"
#include "services/raycast/raycast-store.hpp"
#include <QFutureWatcher>
#include <QTimer>

class RaycastStoreSectionListModel : public SectionListModel {
  Q_OBJECT

public:
  enum ExtraRole {
    DownloadCount = SectionListModel::Accessory + 1,
    AuthorAvatar,
    IsInstalled,
    CompatTierRole,
  };

  using SectionListModel::SectionListModel;

  void setSection(RaycastStoreSection *section) { m_section = section; }

  QHash<int, QByteArray> roleNames() const override;
  QVariant data(const QModelIndex &index, int role) const override;

private:
  RaycastStoreSection *m_section = nullptr;
};

class RaycastStoreViewHost : public ViewHostBase {
  Q_OBJECT
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)

signals:

public:
  RaycastStoreViewHost();

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;
  void onReactivated() override;

  QObject *listModel() const { return const_cast<RaycastStoreSectionListModel *>(&m_model); }

private:
  void fetchExtensions();
  void handleFinishedPage();
  void handleFinishedQuery();
  void handleFinishedCompat();
  void handleDebounce();
  void refresh();
  void tryPopulateModel();

  RaycastStoreSectionListModel m_model{this};
  RaycastStoreSection m_section;
  RaycastStoreService *m_store = nullptr;
  QFutureWatcher<Raycast::ListResult> m_listResultWatcher;
  QFutureWatcher<Raycast::ListResult> m_queryResultWatcher;
  QFutureWatcher<Raycast::CompatResult> m_compatResultWatcher;
  QString m_lastQueryText;
  QTimer m_debounce;
  std::optional<Raycast::ListResponse> m_pendingPage;
  bool m_compatReady = false;
};
