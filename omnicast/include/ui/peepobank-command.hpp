#pragma once
#include "actions/app/app-actions.hpp"
#include "base-view.hpp"
#include "common.hpp"
#include "icon-browser-command.hpp"
#include "omni-icon.hpp"
#include "ui/image/omnimg.hpp"
#include "common-actions.hpp"
#include "service-registry.hpp"
#include "ui/omni-grid.hpp"
#include <qevent.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qpixmap.h>
#include <qmovie.h>
#include <qwindowdefs.h>

class PeepobankView : public GridView {
  class ReplaceAction : public AbstractAction {
    void execute() override { ServiceRegistry::instance()->UI()->replaceCurrentView(new IconBrowserView); }

  public:
    ReplaceAction() : AbstractAction("Replace with icons", BuiltinOmniIconUrl("stars")) {}
  };

  QString bankPath = "/home/aurelle/Pictures/peepobank/";
  struct PeepoInfo {
    QString name;
    QString path;
  };

  class PeepoItem : public OmniGrid::AbstractGridItem, public GridView::Actionnable {
    PeepoInfo _info;
    std::shared_ptr<Application> _fileBrowser;

    QString navigationTitle() const override { return _info.name.split(".").at(0); }

    QString generateId() const override { return _info.path; }

    QString tooltip() const override { return _info.name; }

    double aspectRatio() const override { return 2.0 / 3.0; }

    bool centerWidgetRecyclable() const override { return false; }

    QList<AbstractAction *> generateActions() const override {
      return {new PasteAction(Clipboard::File{_info.path.toStdString()}),
              new OpenAppAction(_fileBrowser, "Open in file browser", {_info.path}), new ReplaceAction};
    }

    void recycleCenterWidget(QWidget *base) const override {
      auto label = static_cast<Omnimg::ImageWidget *>(base);

      label->setUrl(LocalOmniIconUrl(_info.path));
    }

    QWidget *centerWidget() const override {
      auto label = new Omnimg::ImageWidget();

      label->setUrl(LocalOmniIconUrl(_info.path));
      label->setObjectFit(Omnimg::ObjectFitContain);

      return label;
    }

  public:
    const QString &name() const { return _info.name; }
    PeepoItem(const PeepoInfo &info, const std::shared_ptr<Application> &fileBrowser)
        : _info(info), _fileBrowser(fileBrowser) {}
  };

  void onSearchChanged(const QString &text) override {
    auto fileBrowser = ServiceRegistry::instance()->appDb()->fileBrowser();
    QDir dir(bankPath);

    m_grid->updateModel([&]() {
      auto &section = m_grid->addSection("Results");

      section.setColumns(6);
      section.setSpacing(10);

      for (auto entry : dir.entryList()) {
        if (entry.startsWith(".")) continue;
        if (!entry.contains(text, Qt::CaseInsensitive)) { continue; }

        section.addItem(std::make_unique<PeepoItem>(
            PeepoInfo{
                .name = entry,
                .path = dir.filePath(entry),
            },
            fileBrowser));
      }
    });
  }

  void initialize() override {
    QTimer::singleShot(0, [this]() { onSearchChanged(searchText()); });
  }

public:
  PeepobankView() {}
};
