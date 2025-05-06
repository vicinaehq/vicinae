#include "ai/ai-provider.hpp"
#include "app.hpp"
#include "command.hpp"
#include "create-quicklink-command.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action-item.hpp"
#include "require-ai-config-empty-view.hpp"
#include "QScrollArea"
#include "ui/markdown/markdown-renderer.hpp"
#include "ui/omni-scroll-bar.hpp"
#include "ui/toast.hpp"
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
  bool isGenerating = false;
  QuickChatScrollView *chatView;

  void messageSubmission(const QString &text) {
    auto aiManager = ServiceRegistry::instance()->AI();
    auto models = aiManager->listModels();

    if (isGenerating) return;

    auto completion = aiManager->createCompletion(text);

    if (!completion) {
      app.statusBar->setToast("Completion couldn't be created", ToastPriority::Danger);
      return;
    }

    clearSearchText();
    setLoading(true);
    setNavigationTitle("Ask AI - Generating...");

    isGenerating = true;

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
  AskAiCommandView(AppWindow &app) : View(app), chatView(new QuickChatScrollView) {
    auto layout = new QVBoxLayout;

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(chatView);
    setLayout(layout);
  }

  void onMount() override {
    auto aiManager = ServiceRegistry::instance()->AI();

    auto cb = [this](AppWindow &app) { messageSubmission(searchText()); };
    auto action = std::make_shared<CallbackAction>(cb);
    action->setShortcut({.key = "return"});

    std::vector<ActionItem> items;

    items.push_back(std::move(action));
    setActionPannel(items);
  }
};

class AskAiCommand : public CommandContext {
  void load(const LaunchProps &props) override {
    auto aiManager = ServiceRegistry::instance()->AI();
    NavigationStatus nav{.title = command()->name(), .iconUrl = command()->iconUrl()};

    if (aiManager->isAvailable()) {
      return app()->pushView(new AskAiCommandView(*app()), {.navigation = nav});
    }

    app()->pushView(new RequireAiConfigEmptyView(*app()), {.navigation = nav});
  }

public:
  AskAiCommand(AppWindow *app, const std::shared_ptr<AbstractCmd> &command) : CommandContext(app, command) {}
};
