#pragma once
#include "bridge-view.hpp"
#include "image-url.hpp"
#include "services/ai/ai-provider.hpp"
#include "ui/image/url.hpp"
#include "vicinae.hpp"
#include <QVariantList>
#include <memory>
#include <vector>

namespace AI {
class Service;
};

class QuickAIViewHost : public ViewHostBase {
  Q_OBJECT

  Q_PROPERTY(QVariantList exchanges READ exchanges NOTIFY exchangesChanged)
  Q_PROPERTY(bool streaming READ streaming NOTIFY streamingChanged)
  Q_PROPERTY(QString streamingQuery READ streamingQuery NOTIFY streamingChanged)
  Q_PROPERTY(QString streamingContent READ streamingContent NOTIFY streamingContentChanged)
  Q_PROPERTY(QString modelLabel READ modelLabel NOTIFY modelChanged)
  Q_PROPERTY(ImageUrl modelIcon READ modelIcon NOTIFY modelChanged)

signals:
  void exchangesChanged();
  void streamingChanged();
  void streamingContentChanged();
  void modelChanged();

public:
  explicit QuickAIViewHost(QString initialQuery);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;

  QString initialNavigationTitle() const override { return QStringLiteral("Quick AI"); }
  ImageURL initialNavigationIcon() const override {
    return ImageURL::builtin("stars").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  QVariantList exchanges() const { return m_exchanges; }
  bool streaming() const { return m_streaming; }
  QString streamingQuery() const { return m_streamingQuery; }
  QString streamingContent() const { return m_streamingContent; }
  QString modelLabel() const { return m_modelLabel; }
  ImageUrl modelIcon() const { return m_modelIcon; }

private:
  void sendQuery(const std::string &query);
  void updateActions();
  void pasteLastResponse();

  AI::Service *m_aiService = nullptr;
  std::shared_ptr<AI::AbstractChatCompletionStream> m_stream;
  AI::ChatHistory m_history;
  QVariantList m_exchanges;

  QString m_initialQuery;
  QString m_followUpText;
  QString m_streamingQuery;
  std::string m_currentResponse;
  QString m_streamingContent;
  QString m_modelLabel;
  ImageUrl m_modelIcon;
  bool m_streaming = false;
};
