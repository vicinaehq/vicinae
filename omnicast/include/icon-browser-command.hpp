#pragma once
#include <qlabel.h>
#include <qnamespace.h>

/*

class IconBrowserView : public GridView {

  class IconBrowserItem : public AbstractGridItem {
    QString m_name, m_displayName;

    QString tooltip() const override { return m_displayName; }

    QWidget *centerWidget() const override {
      auto iconLabel = new QLabel;

      iconLabel->setPixmap(QIcon::fromTheme(m_name).pixmap({32, 32}));

      return iconLabel;
    }

    int key() const override { return qHash(m_name); }

  public:
    const QString &name() { return m_name; }
    const QString &displayName() { return m_displayName; }

    IconBrowserItem(const QString &name, const QString &displayName)
        : m_name(name), m_displayName(displayName) {}
  };

  std::vector<QSharedPointer<IconBrowserItem>> items;

  void onSearchChanged(const QString &s) override {
    grid->clearContents();

    auto icons = grid->section("Icons");

    icons->setColumns(8);
    icons->setSpacing(10);

    for (const auto &item : items) {
      if (item->displayName().contains(s, Qt::CaseInsensitive)) { icons->addItem(item); }
    }

    grid->calculateLayout();
    grid->selectFirst();
  }

  void onMount() override {
    for (const auto &icon : BuiltinIconService::icons()) {
      auto ss = icon.split(".");
      ss = ss.at(0).split("/");

      auto displayName = ss.at(ss.size() - 1);

      items.push_back(
          QSharedPointer<IconBrowserItem>(new IconBrowserItem(icon, displayName), &QObject::deleteLater));
    }
  }

public:
  IconBrowserView(AppWindow &app) : GridView(app) {}
  ~IconBrowserView() {}
};
*/
