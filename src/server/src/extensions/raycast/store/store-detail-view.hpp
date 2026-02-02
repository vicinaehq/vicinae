#pragma once
#include "actions/extension/extension-actions.hpp"
#include "ui/vertical-scroll-area/vertical-scroll-area.hpp"
#include "ui/views/base-view.hpp"
#include "ui/text-link/text-link.hpp"
#include "ui/image/url.hpp"
#include "services/raycast/raycast-store.hpp"
#include "ui/list-accessory/list-accessory.hpp"
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

class RaycastStoreDetailView : public BaseView {
public:
  RaycastStoreDetailView(const Raycast::Extension &extension) : m_ext(extension) { setupUI(extension); }

private:
  VerticalScrollArea *m_scrollArea = new VerticalScrollArea(this);
  Raycast::Extension m_ext;
  ListAccessoryWidget *m_installedAccessory = new ListAccessoryWidget;

  QString initialNavigationTitle() const override;
  bool supportsSearch() const override;
  Stack createContributorList();

  Stack createHeader();
  Stack createMainWidget();
  QWidget *createPresentationSection();
  QWidget *createSideMetadataSection();
  QWidget *createContentSection();
  void handleClickedScreenshot(const ImageURL &url);
  QWidget *createUI(const Raycast::Extension &ext);

  void extensionInstalled() { m_installedAccessory->show(); }
  void extensionUninstalled() { m_installedAccessory->hide(); }
  void createActions();
  void initialize() override;
  void setupUI(const Raycast::Extension &extension);
};
