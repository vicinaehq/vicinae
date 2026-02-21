#pragma once
#include "extend/model-parser.hpp"
#include "extension/extension-action-panel-builder.hpp"
#include "extension/extension-command-controller.hpp"
#include "bridge-view.hpp"
#include "extension-form-model.hpp"
#include "extension-grid-model.hpp"
#include "extension-list-model.hpp"
#include <QTimer>
#include <variant>

class ExtensionViewHost : public ViewHostBase {
  Q_OBJECT
  Q_PROPERTY(QString viewType READ viewType NOTIFY viewTypeChanged)
  Q_PROPERTY(QObject *contentModel READ contentModel NOTIFY viewTypeChanged)
  Q_PROPERTY(bool isExtLoading READ isExtLoading NOTIFY isLoadingChanged)
  Q_PROPERTY(bool selectFirstOnReset READ selectFirstOnReset NOTIFY selectFirstOnResetChanged)
  Q_PROPERTY(QString detailMarkdown READ detailMarkdown NOTIFY detailContentChanged)
  Q_PROPERTY(QVariantList detailMetadata READ detailMetadata NOTIFY detailContentChanged)
  Q_PROPERTY(bool suppressEmptyView READ suppressEmptyView NOTIFY suppressEmptyViewChanged)
  Q_PROPERTY(QString linkAccessoryText READ linkAccessoryText NOTIFY linkAccessoryChanged)
  Q_PROPERTY(QString linkAccessoryHref READ linkAccessoryHref NOTIFY linkAccessoryChanged)
  Q_PROPERTY(QVariantList dropdownItems READ dropdownItems NOTIFY dropdownChanged)
  Q_PROPERTY(QVariant dropdownCurrentItem READ dropdownCurrentItem NOTIFY dropdownChanged)
  Q_PROPERTY(QString dropdownPlaceholder READ dropdownPlaceholder NOTIFY dropdownChanged)

public:
  explicit ExtensionViewHost(ExtensionCommandController *controller, QObject *parent = nullptr);

  QUrl qmlComponentUrl() const override;
  QUrl qmlSearchAccessoryUrl() const override;
  QVariantMap qmlProperties() const override;
  void loadInitialData() override;
  void onReactivated() override;

  void render(const RenderModel &model);

  void textChanged(const QString &text) override;
  bool inputFilter(QKeyEvent *event) override;
  void beforePop() override;

  QString viewType() const;
  QObject *contentModel() const;
  bool isExtLoading() const;
  bool selectFirstOnReset() const { return m_selectFirstOnReset; }
  QString detailMarkdown() const;
  QVariantList detailMetadata() const;
  bool suppressEmptyView() const { return m_isLoading && !m_hasSearchText; }
  QString linkAccessoryText() const;
  QString linkAccessoryHref() const;
  QVariantList dropdownItems() const { return m_dropdownItems; }
  QVariant dropdownCurrentItem() const { return m_dropdownCurrentItem; }
  QString dropdownPlaceholder() const { return m_dropdownPlaceholder; }

  Q_INVOKABLE void setDropdownValue(const QString &value);

signals:
  void selectFirstOnResetChanged();
  void viewTypeChanged();
  void isLoadingChanged();
  void detailContentChanged();
  void suppressEmptyViewChanged();
  void linkAccessoryChanged();
  void dropdownChanged();

private:
  struct DetailState {
    QString markdown;
    QVariantList metadata;
    std::optional<ActionPannelModel> actions;
  };

  using ViewModel = std::variant<std::monostate, ExtensionListModel *, ExtensionGridModel *,
                                 ExtensionFormModel *, DetailState>;

  void switchViewType(const RenderModel &model);
  void renderList(const ListModel &model);
  void renderGrid(const GridModel &model);
  void renderDetail(const RootDetailModel &model);
  void renderForm(const FormModel &model);
  void notifyExtension(const QString &handler, const QJsonArray &args);
  void handleDebouncedSearch();
  void updateDropdown(const DropdownModel *dropdown);

  template <typename T> T *activeModel() const {
    auto p = std::get_if<T *>(&m_model);
    return p ? *p : nullptr;
  }

  ExtensionCommandController *m_controller;
  ViewModel m_model;
  int m_renderIndex = -1;
  QTimer *m_searchDebounce;
  bool m_isLoading = true;
  bool m_hasSearchText = false;
  bool m_firstRender = true;

  bool m_throttle = false;
  bool m_filtering = false;
  std::optional<std::string> m_onSearchTextChange;
  bool m_shouldResetSelection = false;
  bool m_selectFirstOnReset = true;

  mutable ExtensionActionPanelBuilder::SubmenuCache m_submenuCache;

  std::optional<ActionPannelModel> m_formActions;
  QString m_linkAccessoryText;
  QString m_linkAccessoryHref;

  QVariantList m_dropdownItems;
  QVariant m_dropdownCurrentItem;
  QString m_dropdownValue;
  QString m_dropdownPlaceholder;
  std::optional<QString> m_dropdownOnChange;
};
