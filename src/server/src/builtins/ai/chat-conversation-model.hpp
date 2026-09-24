#pragma once

#include <QAbstractListModel>
#include <QElapsedTimer>
#include <QTimer>
#include <QVariantList>
#include <deque>
#include <optional>
#include <string_view>
#include <string>
#include <vector>
#include <variant>

class ChatConversationModel : public QAbstractListModel {
  Q_OBJECT

signals:
  void contentAdded(int exchange, int content);
  void contentChanged(int exchange, int content);

public:
  struct Response {
    std::string text;
    std::size_t visibleBytes = 0;
    bool restored = false;
  };

  struct Tool {
    quint64 id;
    QString name;
    QString iconSource;
    QString arguments;
    std::optional<QString> summary;
    QString status = QStringLiteral("queued");
    std::optional<QString> output;
    std::optional<qint64> durationMs;
    std::optional<QString> statusText;
    bool expanded = false;
  };

  struct ToolGroup {
    std::vector<Tool> calls;
    bool expanded = false;
  };

  using Content = std::variant<Response, ToolGroup>;

  enum Role { QueryRole = Qt::UserRole + 1, ResponseRole, ErrorRole, PendingRole, AttachmentsRole };

  explicit ChatConversationModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  void beginExchange(const std::string &query, QVariantList attachments = {});
  void appendResponse(std::string_view text);
  void appendSavedResponse(std::string text);
  void finishExchange(const std::string &error = {});

  void addTool(Tool tool);
  void updateTool(quint64 id, QString status, std::optional<QString> output,
                  std::optional<qint64> durationMs = {}, std::optional<QString> statusText = {});
  void toggleTool(quint64 id);
  void toggleToolGroup(quint64 id);
  const std::vector<Content> &contents(int exchange) const { return m_exchanges[exchange].contents; }

private:
  struct Exchange {
    std::string query;
    std::vector<Content> contents;
    std::string error;
    bool pending = true;
    QVariantList attachments;
  };

  struct RevealBatch {
    qsizetype begin;
    qsizetype end;
    qint64 receivedAt;
    qint64 duration;
  };

  void advanceResponse();
  void flushResponse();
  void publishResponse();

  std::vector<Exchange> m_exchanges;

  QTimer m_responseUpdateTimer;
  QElapsedTimer m_streamClock;
  QString m_pendingText;
  std::optional<qint64> m_lastUpdate;
  std::optional<qint64> m_lastArrival;
  double m_revealDelay = 0;
  std::deque<RevealBatch> m_revealBatches;
  qsizetype m_receivedUnits = 0;
};
