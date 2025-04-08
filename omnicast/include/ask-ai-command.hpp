#include "ai/ai-provider.hpp"
#include "app.hpp"
#include "create-quicklink-command.hpp"
#include "ui/action-pannel/action-item.hpp"
#include "QScrollArea"
#include "ui/markdown/markdown-renderer.hpp"
#include "ui/omni-scroll-bar.hpp"
#include "view.hpp"
#include <qboxlayout.h>
#include <qcoreevent.h>
#include <qevent.h>
#include <qfuturewatcher.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qrunnable.h>
#include <qscrollarea.h>
#include <qwidget.h>

class QuickChatBubble : public QWidget {
  MarkdownRenderer *renderer;
  QVBoxLayout *layout;

  void paintEvent(QPaintEvent *event) override {
    auto &theme = ThemeService::instance().theme();
    int borderRadius = 5;
    QPainter painter(this);
    QPainterPath path;
    QPen pen(theme.colors.statusBackgroundBorder, 1);

    painter.setRenderHint(QPainter::Antialiasing, true);
    path.addRoundedRect(rect(), borderRadius, borderRadius);

    painter.setClipPath(path);

    QColor finalColor(theme.colors.mainBackground);

    finalColor.setAlphaF(0.98);
    painter.setPen(pen);
    painter.fillPath(path, finalColor);
    painter.drawPath(path);
  }

public:
  QuickChatBubble(QWidget *parent = nullptr) : renderer(new MarkdownRenderer), layout(new QVBoxLayout) {
    layout->addWidget(renderer);
    setMinimumHeight(0);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    setLayout(layout);
  }

  void appendMarkdown(const QString &markdown) { renderer->appendMarkdown(markdown); }
};

class QuickChatView : public QWidget {
  QVBoxLayout *layout;

public:
  QuickChatBubble *bubble;

  QuickChatView() : layout(new QVBoxLayout), bubble(new QuickChatBubble) {
    layout->addWidget(bubble);
    layout->addStretch();
    setLayout(layout);
  }
};

class QuickChatScrollView : public QScrollArea {
  void mouseMoveEvent(QMouseEvent *event) override {
    qDebug() << "mouse position" << event->position();

    int diff = event->pos().y() - (pos().y() + height());

    if (diff > 0) {
      qDebug() << "below";
      verticalScrollBar()->setValue(verticalScrollBar()->value() + diff);
    }

    QWidget::mouseMoveEvent(event);
  }

public:
  QuickChatView *view;
  QuickChatScrollView(QWidget *parent = nullptr) : QScrollArea(parent), view(new QuickChatView()) {
    setVerticalScrollBar(new OmniScrollBar);
    setWidget(view);
    setWidgetResizable(true);
    setAutoFillBackground(false);
    view->setAutoFillBackground(false);
    viewport()->setAutoFillBackground(false);
    setAttribute(Qt::WA_TranslucentBackground);
  }
};

class AskAiCommandView : public View {
  Service<AbstractAiProvider> aiProvider;
  std::vector<AiModel> models;
  bool isGenerating = false;
  QuickChatScrollView *chatView;

  void messageSubmission(const QString &text) {
    if (models.empty()) {
      qDebug() << "empty list of models";
      return;
    }

    if (isGenerating) return;

    clearSearchText();
    setLoading(true);
    setNavigationTitle("Ask AI - Generating...");

    isGenerating = true;
    ChatMessage prompt(text);
    auto completion =
        aiProvider.createStreamedCompletion({.modelId = "mistral:latest", .messages = {prompt}});

    connect(completion, &StreamedChatCompletion::tokenReady, this, [this](const ChatCompletionToken &token) {
      chatView->view->bubble->appendMarkdown(token.message.content().toString());
    });
    connect(completion, &StreamedChatCompletion::finished, this, [this]() {
      isGenerating = false;
      setLoading(false);
      setNavigationTitle("Ask AI");
    });
    connect(completion, &StreamedChatCompletion::errorOccured, this, [this](const QString &error) {
      isGenerating = false;
      setNavigationTitle("Ask AI");
      setLoading(false);
      qDebug() << "Failed to stream" << error;
    });

    qDebug() << "model to use:" << models.at(0).displayName;
  }

public:
  AskAiCommandView(AppWindow &app)
      : View(app), aiProvider(*app.aiProvider.get()), chatView(new QuickChatScrollView) {
    aiProvider.models().then([this](std::vector<AiModel> models) { this->models = models; });

    auto layout = new QVBoxLayout;

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(chatView);
    setLayout(layout);
  }

  void onMount() override {
    auto cb = [this](AppWindow &app) { messageSubmission(searchText()); };
    auto action = std::make_unique<CallbackAction>(cb);
    action->setShortcut({.key = "return"});

    std::vector<ActionItem> items;

    items.push_back(std::move(action));
    setActionPannel(std::move(items));
  }
};
