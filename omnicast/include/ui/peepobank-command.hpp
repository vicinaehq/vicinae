#pragma once
#include "app.hpp"
#include "common-actions.hpp"
#include "service-registry.hpp"
#include "ui/omni-grid-view.hpp"
#include "ui/omni-grid.hpp"
#include <qevent.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qpixmap.h>
#include <qmovie.h>
#include <qwindowdefs.h>
#include <variant>

class PeepobankView : public OmniGridView {
  QString bankPath = "/home/aurelle/Pictures/peepobank/";
  struct PeepoInfo {
    QString name;
    QString path;
  };

  class PeepoLabel : public QLabel {
    std::variant<std::monostate, QPixmap, QMovie *> pixmap;

    void resizeEvent(QResizeEvent *event) override {
      QLabel::resizeEvent(event);
      recalculate();
    }

    void recalculate() {
      if (auto pix = std::get_if<QPixmap>(&pixmap); pix) {
        setPixmap(pix->scaled(width(), height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
      }
      if (auto movie = std::get_if<QMovie *>(&pixmap); movie) {
        (*movie)->setScaledSize(QSize{width(), height()});
      }
    }

  public:
    void setPeepo(PeepoInfo info) {
      if (auto movie = std::get_if<QMovie *>(&pixmap); movie) { (*movie)->deleteLater(); }

      if (info.name.endsWith(".gif")) {
        auto newMovie = new QMovie(info.path);

        newMovie->start();
        setMovie(newMovie);
        pixmap = newMovie;
      } else {
        pixmap = QPixmap(info.path);
      }

      recalculate();
    }

    ~PeepoLabel() {
      if (auto movie = std::get_if<QMovie *>(&pixmap); movie) { (*movie)->deleteLater(); }
    }

    PeepoLabel() : pixmap(std::monostate{}) { setAlignment(Qt::AlignCenter); }
  };

  class PeepoItem : public OmniGrid::AbstractGridItem, public OmniGridView::IActionnable {
    PeepoInfo _info;
    std::shared_ptr<Application> _fileBrowser;

    QString navigationTitle() const override { return _info.name.split(".").at(0); }

    QString id() const override { return _info.path; }

    QString tooltip() const override { return _info.name; }

    bool centerWidgetRecyclable() const override { return true; }

    QList<AbstractAction *> generateActions() const override {
      return {new PasteAction(Clipboard::File{_info.path.toStdString()}),
              new OpenAppAction(_fileBrowser, "Open in file browser", {_info.path})};
    }

    void recycleCenterWidget(QWidget *base) const override {
      auto label = static_cast<PeepoLabel *>(base);

      label->setPeepo(_info);
    }

    QWidget *centerWidget() const override {
      auto label = new PeepoLabel;

      label->setPeepo(_info);

      return label;
    }

  public:
    const QString &name() const { return _info.name; }
    PeepoItem(const PeepoInfo &info, const std::shared_ptr<Application> &fileBrowser)
        : _info(info), _fileBrowser(fileBrowser) {}
  };

  class PeepoFilter : public OmniList::AbstractItemFilter {
    QString query;

    bool matches(const OmniList::AbstractVirtualItem &item) override {
      auto &peepo = static_cast<const PeepoItem &>(item);

      return peepo.name().contains(query, Qt::CaseInsensitive);
    }

  public:
    PeepoFilter(const QString &query) : query(query) {}
  };

  void onMount() override {
    OmniGridView::onMount();
    auto fileBrowser = ServiceRegistry::instance()->appDb()->fileBrowser();
    QDir dir(bankPath);

    grid->beginUpdate();
    grid->addSection("Results");

    for (auto entry : dir.entryList()) {
      if (entry.startsWith(".")) continue;

      grid->addItem(std::make_unique<PeepoItem>(
          PeepoInfo{
              .name = entry,
              .path = dir.filePath(entry),
          },
          fileBrowser));
    }

    grid->commitUpdate();
    grid->selectFirst();
  }

  void onSearchChanged(const QString &text) override { grid->setFilter(std::make_unique<PeepoFilter>(text)); }

public:
  PeepobankView(AppWindow &app) : OmniGridView(app) { grid->setColumns(8); }
};
