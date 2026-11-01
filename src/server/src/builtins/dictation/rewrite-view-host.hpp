#pragma once
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <QCoreApplication>
#include <QString>
#include "builtins/dictation/styles-view-host.hpp"
#include "services/dictation/dictation-rewrite.hpp"

/**
 * Tries styles on a past transcript, so instructions and models can be compared without dictating.
 */
class DictationRewriteViewHost : public MonoListViewHost<DictationStyles::Style> {
  Q_DECLARE_TR_FUNCTIONS(DictationRewriteViewHost)

public:
  explicit DictationRewriteViewHost(QString text) : m_text(std::move(text)) {}

  void onMount() override;
  QString initialNavigationTitle() const override { return tr("Rewrite with Style"); }
  ImageURL initialNavigationIcon() const override;

  QString displayId(const ItemType &style) const override { return QString::fromStdString(style.id); }
  QString displayTitle(const ItemType &style) const override { return QString::fromStdString(style.name); }
  QString displaySubtitle(const ItemType &) const override { return {}; }
  std::optional<ImageURL> displayIcon(const ItemType &) const override;
  AccessoryList displayAccessories(const ItemType &style) const override;
  bool hasDetailPane() const override { return true; }
  std::optional<ListItemDetail> displayDetail(const ItemType &style) const override;
  std::unique_ptr<ActionPanelState> buildActionPanel(const ItemType &style) const override;
  void beforePop() override;

protected:
  std::unique_ptr<ActionPanelState> emptyActionPanel() override;

private:
  struct Run {
    DictationRewrite *rewrite = nullptr;
    std::optional<QString> result;
    std::optional<QString> error;
  };

  void start(const ItemType &style);
  void refresh();

  QString m_text;
  std::map<std::string, Run> m_runs;
};
