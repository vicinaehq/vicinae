#pragma once
#include "actions/extension/extension-actions.hpp"
#include "services/extension-store/vicinae-store.hpp"
#include "ui/image/url.hpp"
#include "ui/list-accessory/list-accessory.hpp"
#include "ui/text-link/text-link.hpp"
#include "ui/vertical-scroll-area/vertical-scroll-area.hpp"
#include "ui/views/base-view.hpp"
#include "utils/layout.hpp"
#include <qboxlayout.h>
#include <qevent.h>
#include <qfuturewatcher.h>
#include <qnamespace.h>
#include <qscrollarea.h>
#include <qsizepolicy.h>
#include <qtmetamacros.h>
#include <qurl.h>
#include <qwidget.h>

class VicinaeStoreDetailView : public BaseView {
public:
  VicinaeStoreDetailView(const VicinaeStore::Extension &extension) : m_ext(extension) { setupUI(extension); }

private:
  VerticalScrollArea *m_scrollArea = new VerticalScrollArea(this);
  VicinaeStore::Extension m_ext;
  ListAccessoryWidget *m_installedAccessory = new ListAccessoryWidget;

  QString initialNavigationTitle() const override;
  bool supportsSearch() const override;

  Stack createHeader();
  Stack createMainWidget();
  QWidget *createPresentationSection();
  QWidget *createSideMetadataSection();
  QWidget *createContentSection();
  QWidget *createUI(const VicinaeStore::Extension &ext);

  void extensionInstalled() { m_installedAccessory->show(); }
  void extensionUninstalled() { m_installedAccessory->hide(); }
  void createActions();
  void initialize() override;
  void setupUI(const VicinaeStore::Extension &extension);
};
