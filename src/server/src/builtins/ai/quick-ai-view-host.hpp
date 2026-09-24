#pragma once

#include <QPointer>
#include "chat-session.hpp"
#include "ui/views/bridge-view.hpp"

class QuickAIViewHost : public ViewHostBase {
  Q_OBJECT
  QML_NAMED_ELEMENT(QuickAIViewHost)
  QML_UNCREATABLE("")
  Q_PROPERTY(ChatSession *session READ session CONSTANT)

public:
  Q_INVOKABLE void openInWindow();

public:
  QuickAIViewHost(QString initialQuery, std::optional<AI::ModelRef> model);
  static void openConversation(BaseView *source, std::string id);
  ~QuickAIViewHost() override;
  void beforePop() override;
  void initialize() override;
  void loadInitialData() override;
  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  bool supportsSearch() const override { return false; }
  bool needsGlobalStatusBar() const override { return false; }
  QString initialNavigationTitle() const override;
  ImageURL initialNavigationIcon() const override;
  ChatSession *session() const { return m_session; }

private:
  QString m_initialQuery;
  QPointer<ChatSession> m_session;
};
