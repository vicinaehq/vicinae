#pragma once
#include "ui/image/image-url.hpp"
#include "ui/views/list-view-host.hpp"
#include <QUrl>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>

class DetailListViewHost : public ListViewHost {
  Q_OBJECT
  QML_NAMED_ELEMENT(DetailListViewHost)
  QML_UNCREATABLE("")
  Q_PROPERTY(bool hasDetail READ hasDetail NOTIFY detailChanged)
  Q_PROPERTY(QString detailContent READ detailContent NOTIFY detailChanged)
  Q_PROPERTY(QUrl detailContentUrl READ detailContentUrl NOTIFY detailChanged)
  Q_PROPERTY(QVariantList detailMetadata READ detailMetadata NOTIFY detailChanged)
  Q_PROPERTY(bool detailMarkdown READ detailMarkdown NOTIFY detailChanged)
  Q_PROPERTY(QString emptyTitle READ emptyTitle CONSTANT)
  Q_PROPERTY(QString emptyDescription READ emptyDescription CONSTANT)
  Q_PROPERTY(ImageUrl emptyIcon READ emptyIcon CONSTANT)

signals:
  void detailChanged();

public:
  virtual bool hasDetail() const = 0;
  virtual QString detailContent() const = 0;
  virtual QUrl detailContentUrl() const { return {}; }
  virtual QVariantList detailMetadata() const = 0;
  virtual bool detailMarkdown() const { return false; }

  QString emptyTitle() const { return m_emptyTitle; }
  QString emptyDescription() const { return m_emptyDescription; }
  ImageUrl emptyIcon() const { return m_emptyIcon; }

protected:
  QString m_emptyTitle;
  QString m_emptyDescription;
  ImageUrl m_emptyIcon;
};
