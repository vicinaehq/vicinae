#pragma once
#include "section-list-model.hpp"
#include "vicinae-store-model.hpp"
#include "bridge-view.hpp"
#include "services/extension-store/vicinae-store.hpp"
#include <QFutureWatcher>

class VicinaeStoreSectionListModel : public SectionListModel {
  Q_OBJECT

public:
  enum ExtraRole {
    DownloadCount = SectionListModel::Accessory + 1,
    AuthorAvatar,
    IsInstalled,
    CompatTierRole,
  };

  using SectionListModel::SectionListModel;

  void setSection(VicinaeStoreSection *section) { m_section = section; }

  QHash<int, QByteArray> roleNames() const override;
  QVariant data(const QModelIndex &index, int role) const override;

private:
  VicinaeStoreSection *m_section = nullptr;
};

class VicinaeStoreViewHost : public ViewHostBase {
  Q_OBJECT
  Q_PROPERTY(QObject *listModel READ listModel CONSTANT)

signals:

public:
  VicinaeStoreViewHost();

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;
  void onReactivated() override;

  QObject *listModel() const { return const_cast<VicinaeStoreSectionListModel *>(&m_model); }

private:
  void fetchExtensions();
  void handleFinished();
  void refresh();

  VicinaeStoreSectionListModel m_model{this};
  VicinaeStoreSection m_section;
  VicinaeStoreService *m_store = nullptr;
  QFutureWatcher<VicinaeStore::ListResult> m_watcher;
};
