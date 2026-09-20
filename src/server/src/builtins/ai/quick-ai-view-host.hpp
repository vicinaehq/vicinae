#pragma once
#include <QtQml/qqmlregistration.h>
#include "ui/views/bridge-view.hpp"
#include "ui/image/image-url.hpp"
#include "services/ai/ai-provider.hpp"
#include "ui/image/url.hpp"
#include "vicinae.hpp"
#include <QTimer>
#include <QVariantList>
#include <QVariantMap>
#include <memory>
#include <optional>
#include <string>

namespace AI {
class Service;
};
class DictationService;
class TranscriptionSession;

class QuickAIViewHost : public ViewHostBase {
  Q_OBJECT
  QML_NAMED_ELEMENT(QuickAIViewHost)
  QML_UNCREATABLE("")

  Q_PROPERTY(QVariantList exchanges READ exchanges NOTIFY exchangesChanged)
  Q_PROPERTY(bool streaming READ streaming NOTIFY streamingChanged)
  Q_PROPERTY(QString streamingQuery READ streamingQuery NOTIFY streamingChanged)
  Q_PROPERTY(QString streamingContent READ streamingContent NOTIFY streamingContentChanged)
  Q_PROPERTY(QString modelLabel READ modelLabel NOTIFY modelChanged)
  Q_PROPERTY(ImageUrl modelIcon READ modelIcon NOTIFY modelChanged)
  Q_PROPERTY(QVariantList modelSelectorItems READ modelSelectorItems NOTIFY modelSelectorItemsChanged)
  Q_PROPERTY(QVariantMap modelSelectorCurrentItem READ modelSelectorCurrentItem NOTIFY
                 modelSelectorCurrentItemChanged)
  Q_PROPERTY(bool dictationAvailable READ dictationAvailable NOTIFY dictationAvailableChanged)
  Q_PROPERTY(bool recording READ recording NOTIFY dictationStateChanged)
  Q_PROPERTY(bool transcribing READ transcribing NOTIFY dictationStateChanged)
  Q_PROPERTY(QString recordingTime READ recordingTime NOTIFY recordingTimeChanged)
  Q_PROPERTY(QString dictationMessage READ dictationMessage NOTIFY dictationMessageChanged)

signals:
  void exchangesChanged();
  void dictationAvailableChanged();
  void dictationStateChanged();
  void recordingTimeChanged();
  void dictationMessageChanged();
  void dictated(const QString &text);
  void streamingChanged();
  void streamingContentChanged();
  void modelChanged();
  void modelSelectorItemsChanged();
  void modelSelectorCurrentItemChanged();

public:
  QuickAIViewHost(QString initialQuery, AI::ModelRef model);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  void initialize() override;
  void loadInitialData() override;

  bool supportsSearch() const override { return false; }
  bool needsGlobalStatusBar() const override { return false; }

  QString initialNavigationTitle() const override { return QStringLiteral("Quick AI"); }
  ImageURL initialNavigationIcon() const override {
    return ImageURL::builtin(BuiltinIcon::Stars).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  QVariantList exchanges() const { return m_exchanges; }
  bool streaming() const { return m_streaming; }
  QString streamingQuery() const { return m_streamingQuery; }
  QString streamingContent() const { return m_streamingContent; }
  QString modelLabel() const { return m_modelLabel; }
  ImageUrl modelIcon() const { return m_modelIcon; }
  QVariantList modelSelectorItems() const { return m_modelSelectorItems; }
  QVariantMap modelSelectorCurrentItem() const { return m_modelSelectorCurrentItem; }

  bool dictationAvailable() const { return m_dictationAvailable; }
  bool recording() const;
  bool transcribing() const;
  QString recordingTime() const;
  QString dictationMessage() const { return m_dictationMessage; }

  Q_INVOKABLE void send(const QString &text);
  Q_INVOKABLE void cancel();
  Q_INVOKABLE void selectModel(const QString &compositeId);
  Q_INVOKABLE void toggleDictation();
  Q_INVOKABLE void cancelDictation();

private:
  void sendQuery(const std::string &query);
  void failQuery(const std::string &reason);
  void rebuildModelSelectorItems();
  void updateDictationAvailable();
  void startDictation();
  void showDictationMessage(const QString &message);

  AI::Service *m_aiService = nullptr;
  DictationService *m_dictationService = nullptr;
  TranscriptionSession *m_dictation = nullptr;
  QTimer m_dictationMessageTimer;
  bool m_dictationAvailable = false;
  QString m_dictationMessage;
  std::shared_ptr<AI::AbstractChatCompletionStream> m_stream;
  AI::ChatHistory m_history;
  QVariantList m_exchanges;

  QString m_initialQuery;
  QString m_streamingQuery;
  std::string m_currentResponse;
  QString m_streamingContent;
  QString m_modelLabel;
  ImageUrl m_modelIcon;
  bool m_streaming = false;

  std::optional<AI::ModelRef> m_selectedModel;
  QVariantList m_modelSelectorItems;
  QVariantMap m_modelSelectorCurrentItem;
};
