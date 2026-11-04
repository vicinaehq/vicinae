#pragma once

#include <QPointer>
#include "chat-session.hpp"
#include "ui/views/bridge-view.hpp"

class QuickAIViewHost : public ViewHostBase {
  Q_OBJECT
  QML_NAMED_ELEMENT(QuickAIViewHost)
  QML_UNCREATABLE("")
  Q_PROPERTY(ChatSession *session READ session CONSTANT)
  Q_PROPERTY(bool opening READ opening NOTIFY openingChanged)

public:
  Q_INVOKABLE void openInWindow();

signals:
  void openingChanged();

public:
  explicit QuickAIViewHost(QString initialQuery = {});
  static QuickAIViewHost *forLastConversation();
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
  bool opening() const { return m_opening; }

private:
  void openLastConversation();
  void setOpening(bool value);
  void leaveWithError(const QString &message);

  QString m_initialQuery;
  QPointer<ChatSession> m_session;
  bool m_opening = false;
};
