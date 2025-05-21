#pragma once
#include "base-view.hpp"
#include "omni-icon.hpp"
#include "ui/image/omnimg.hpp"
#include "app-root-provider.hpp"
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

class PeepobankView : public GridView {
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

    bool centerWidgetRecyclable() const override { return false; }

    QList<AbstractAction *> generateActions() const override {
      return {new PasteAction(Clipboard::File{_info.path.toStdString()}),
              new OpenAppAction(_fileBrowser, "Open in file browser", {_info.path})};
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

  class PeepoFilter : public OmniList::AbstractItemFilter {
    QString query;

    bool matches(const OmniList::AbstractVirtualItem &item) override {
      auto &peepo = static_cast<const PeepoItem &>(item);

      return peepo.name().contains(query, Qt::CaseInsensitive);
    }

  public:
    PeepoFilter(const QString &query) : query(query) {}
  };

  void onSearchChanged(const QString &text) override {
    m_grid->setFilter(std::make_unique<PeepoFilter>(text));
  }

public:
  PeepobankView() {
    auto fileBrowser = ServiceRegistry::instance()->appDb()->fileBrowser();
    QDir dir(bankPath);

    m_grid->setColumns(8);
    m_grid->beginUpdate();
    m_grid->addSection("Results");

    for (auto entry : dir.entryList()) {
      if (entry.startsWith(".")) continue;

      m_grid->addItem(std::make_unique<PeepoItem>(
          PeepoInfo{
              .name = entry,
              .path = dir.filePath(entry),
          },
          fileBrowser));
    }

    m_grid->commitUpdate();
    m_grid->selectFirst();
  }
};
