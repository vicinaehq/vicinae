#pragma once
#include <QAbstractListModel>
#include <QVariantList>

class QmlKeybindSettingsModel : public QAbstractListModel {
  Q_OBJECT

  Q_PROPERTY(QString selectedName READ selectedName NOTIFY selectedChanged)
  Q_PROPERTY(QString selectedDescription READ selectedDescription NOTIFY selectedChanged)
  Q_PROPERTY(QString selectedIcon READ selectedIcon NOTIFY selectedChanged)
  Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY selectedChanged)
  Q_PROPERTY(int selectedRow READ selectedRow NOTIFY selectedChanged)

signals:
  void selectedChanged();

public:
  enum Role { NameRole = Qt::UserRole + 1, IconRole, DescriptionRole, ShortcutRole, KeybindIdRole };

  explicit QmlKeybindSettingsModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  QString selectedName() const;
  QString selectedDescription() const;
  QString selectedIcon() const;
  bool hasSelection() const;
  int selectedRow() const { return m_selectedRow; }

  Q_INVOKABLE void setFilter(const QString &text);
  Q_INVOKABLE void select(int row);
  Q_INVOKABLE void moveUp();
  Q_INVOKABLE void moveDown();
  Q_INVOKABLE QString validateShortcut(int key, int modifiers) const;
  Q_INVOKABLE void setShortcut(int row, int key, int modifiers);
  Q_INVOKABLE QVariantList shortcutKeys(int key, int modifiers) const;

private:
  struct Entry {
    int keybindId;
    QString name;
    QString icon;
    QString description;
    QString shortcut;
  };

  void rebuild(const QString &filter);

  std::vector<Entry> m_entries;
  int m_selectedRow = -1;
};
