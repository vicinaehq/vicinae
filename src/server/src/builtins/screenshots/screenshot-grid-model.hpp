#pragma once

#include <array>
#include <vector>
#include "services/screenshots/screenshot.hpp"
#include "ui/views/section-grid-model.hpp"

class ScreenshotGridSource : public GridSource {
  Q_DECLARE_TR_FUNCTIONS(ScreenshotGridSource)

public:
  QString name;
  std::vector<Screenshot> items;
  QString sectionName() const override { return name; }
  int count() const override { return static_cast<int>(items.size()); }
  QString itemTitle(int i) const override;
  QString itemSubtitle(int i) const override;
  QString itemTooltip(int i) const override;
  std::optional<ImageURL> itemIcon(int i) const override;
  bool isDraggable(int) const override { return true; }
  std::unique_ptr<QMimeData> dragMimeData(int i) const override;
  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;
};

class ScreenshotGridModel : public SectionGridModel {
  Q_OBJECT

public:
  explicit ScreenshotGridModel(QObject *parent = nullptr);
  void initialize();
  void setItems(const std::vector<Screenshot> &items);
  void setFilter(const QString &text);

protected:
  void onSelectionCleared() override;

private:
  void rebuildResults(bool preserveSelection);
  std::vector<Screenshot> m_items;
  std::array<ScreenshotGridSource, 3> m_sections;
  QString m_filter;
};
