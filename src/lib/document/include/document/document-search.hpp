#pragma once

#include <QPointer>
#include <QTimer>
#include <compare>
#include <memory>
#include <stop_token>
#include "document-model.hpp"

namespace vicinae::document {

class DocumentSearch : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("Use the document controller's search")
  Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)
  Q_PROPERTY(QString resultQuery READ resultQuery NOTIFY resultsChanged)
  Q_PROPERTY(int count READ count NOTIFY resultsChanged)
  Q_PROPERTY(int currentIndex READ currentIndex NOTIFY currentIndexChanged)
  Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)

public:
  Q_INVOKABLE void next();
  Q_INVOKABLE void previous();

signals:
  void queryChanged();
  void resultsChanged();
  void currentIndexChanged();
  void busyChanged();
  void revealRequested(int row, int part, int position, int length);

public:
  struct Match {
    int row;
    int part;
    int start;
    int length;
    auto operator<=>(const Match &) const = default;
  };
  explicit DocumentSearch(QObject *parent = nullptr);
  ~DocumentSearch() override;
  void setModel(DocumentModel *model);
  QString query() const { return m_query; }
  QString resultQuery() const { return m_resultQuery; }
  void setQuery(const QString &query);
  int count() const { return int(m_matches.size()); }
  int currentIndex() const { return m_current; }
  bool busy() const { return m_busy; }
  std::span<const Match> matches(int row, int part) const;
  const Match *currentMatch() const;

private:
  struct Row {
    DocumentModel::TextSnapshot snapshot;
    std::vector<DocumentPart> parts;
    QString query;
    std::vector<Match> matches;
  };
  void reset();
  void invalidate(int first, int last);
  void rowsChanged(int first, int removed, int added);
  void schedule();
  void search();
  void move(int direction);
  void setBusy(bool busy);
  QPointer<DocumentModel> m_model;
  std::vector<std::shared_ptr<Row>> m_rows;
  std::vector<Match> m_matches;
  QString m_query;
  QString m_resultQuery;
  QTimer m_timer;
  std::stop_source m_cancellation;
  quint64 m_revision = 0;
  int m_current = -1;
  bool m_running = false;
  bool m_busy = false;
  bool m_reveal = false;
};

} // namespace vicinae::document
