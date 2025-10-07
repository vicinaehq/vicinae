#pragma once
#include <QListWidget>
#include <QTextEdit>
#include <qboxlayout.h>
#include <qdir.h>
#include <qlabel.h>
#include <qlistwidget.h>
#include <qnamespace.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qsizepolicy.h>
#include <qtextedit.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include "extend/model-parser.hpp"
#include "extension/extension-view.hpp"
#include "ui/markdown/markdown-renderer.hpp"
#include "ui/split-detail/split-detail.hpp"
#include "ui/vertical-metadata/vertical-metadata.hpp"
#include "utils/layout.hpp"

class ExtensionDetailView : public ExtensionSimpleView {
  Q_OBJECT

  MarkdownRenderer *markdownEditor;
  VerticalMetadata *metadata;
  SplitDetailWidget *m_split;

  bool supportsSearch() const override { return false; }

public:
  ExtensionDetailView()
      : markdownEditor(new MarkdownRenderer), metadata(new VerticalMetadata()),
        m_split(new SplitDetailWidget) {
    setDefaultActionShortcuts({Keyboard::Shortcut::enter(), Keyboard::Shortcut::submit()});
    m_split->setMainWidget(markdownEditor);
    m_split->setDetailWidget(metadata);
    m_split->setRatio(0.40);
    markdownEditor->textEdit()->setFocusPolicy(Qt::StrongFocus);
    VStack().add(m_split).imbue(this);
  }

  void initialize() override { markdownEditor->textEdit()->setFocus(); }

  void render(const RenderModel &model) override {
    auto newModel = std::get<RootDetailModel>(model);

    setLoading(newModel.isLoading);

    if (newModel.navigationTitle) { setNavigationTitle(*newModel.navigationTitle); }

    markdownEditor->setMarkdown(newModel.markdown);

    if (auto actions = newModel.actions) { setActionPanel(*actions); }

    if (newModel.metadata) {
      metadata->setMetadata(newModel.metadata->children);
    } else {
      metadata->hide();
    }
    m_split->setDetailVisibility(newModel.metadata.has_value());

    if (auto actions = newModel.actions) { setActionPanel(*actions); }
  }
};
