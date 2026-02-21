#pragma once
#include "app-selector-model.hpp"
#include "bridge-view.hpp"
#include <QUrl>
#include <QVariantList>
#include <QVariantMap>
#include <memory>

class ShortcutService;
class Shortcut;

class ShortcutFormViewHost : public FormViewBase {
  Q_OBJECT

public:
  enum class Mode { Create, Edit, Duplicate };

  Q_PROPERTY(QString name READ name WRITE setName NOTIFY formChanged)
  Q_PROPERTY(QString link READ link WRITE setLink NOTIFY formChanged)
  Q_PROPERTY(QVariantMap selectedApp READ selectedApp NOTIFY formChanged)
  Q_PROPERTY(QVariantMap selectedIcon READ selectedIcon NOTIFY formChanged)

  Q_PROPERTY(QString linkError READ linkError NOTIFY errorsChanged)
  Q_PROPERTY(QString appError READ appError NOTIFY errorsChanged)
  Q_PROPERTY(QString iconError READ iconError NOTIFY errorsChanged)

  Q_PROPERTY(AppSelectorModel *appSelectorModel READ appSelectorModel CONSTANT)
  Q_PROPERTY(QVariantList iconItems READ iconItems NOTIFY iconItemsChanged)
  Q_PROPERTY(QVariantList linkCompletions READ linkCompletions CONSTANT)

  ShortcutFormViewHost();
  ShortcutFormViewHost(std::shared_ptr<Shortcut> shortcut, Mode mode);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() const override;
  void initialize() override;

  QString name() const { return m_name; }
  QString link() const { return m_link; }
  QVariantMap selectedApp() const { return m_selectedApp; }
  QVariantMap selectedIcon() const { return m_selectedIcon; }

  QString linkError() const { return m_linkError; }
  QString appError() const { return m_appError; }
  QString iconError() const { return m_iconError; }

  AppSelectorModel *appSelectorModel() const { return m_appSelectorModel; }
  QVariantList iconItems() const { return m_iconItems; }
  QVariantList linkCompletions() const { return m_linkCompletions; }

  void setName(const QString &v) {
    if (m_name != v) {
      m_name = v;
      emit formChanged();
    }
  }
  void setLink(const QString &v) {
    if (m_link != v) {
      m_link = v;
      emit formChanged();
    }
  }

  void setPrefilledValues(const QString &link, const QString &name = "", const QString &application = "",
                          const QString &icon = "");

  Q_INVOKABLE void submit();
  Q_INVOKABLE void handleLinkBlurred();
  Q_INVOKABLE void selectApp(const QVariantMap &item);
  Q_INVOKABLE void selectIcon(const QVariantMap &item);

signals:
  void formChanged();
  void errorsChanged();
  void iconItemsChanged();

private:
  void buildIconItems();
  void buildLinkCompletions();
  void updateDefaultIconInItems();

  Mode m_mode = Mode::Create;
  std::shared_ptr<Shortcut> m_initialShortcut;
  ShortcutService *m_service = nullptr;
  AppSelectorModel *m_appSelectorModel = nullptr;

  QString m_name;
  QString m_link;
  QVariantMap m_selectedApp;
  QVariantMap m_selectedIcon;

  QString m_linkError;
  QString m_appError;
  QString m_iconError;

  QVariantList m_iconItems;
  QVariantList m_linkCompletions;
  QVariantMap m_defaultIconEntry;

  QString m_prefilledLink;
  QString m_prefilledName;
  QString m_prefilledApp;
  QString m_prefilledIcon;
};
