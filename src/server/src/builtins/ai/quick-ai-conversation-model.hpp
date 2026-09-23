#pragma once

#include <QAbstractListModel>
#include <QElapsedTimer>
#include <QTimer>
#include <deque>
#include <optional>
#include <string_view>
#include <string>
#include <vector>

class QuickAIConversationModel : public QAbstractListModel {
  Q_OBJECT

public:
  enum Role { QueryRole = Qt::UserRole + 1, ResponseRole, ErrorRole, PendingRole };

  explicit QuickAIConversationModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  void beginExchange(const std::string &query);
  void appendResponse(std::string_view text);
  void finishExchange(const std::string &error = {});

private:
  struct Exchange {
    std::string query;
    std::string response;
    std::string error;
    std::size_t visibleBytes = 0;
    bool pending = true;
  };

  struct RevealBatch {
    qsizetype begin;
    qsizetype end;
    qint64 receivedAt;
  };

  void advanceResponse();
  void flushResponse();

  std::vector<Exchange> m_exchanges;
  QTimer m_responseUpdateTimer;
  QElapsedTimer m_streamClock;
  QString m_pendingText;
  std::optional<qint64> m_lastUpdate;
  std::deque<RevealBatch> m_revealBatches;
  qsizetype m_receivedUnits = 0;
};
