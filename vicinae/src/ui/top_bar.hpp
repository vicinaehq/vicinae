#pragma once
#include "argument.hpp"
#include "omni-icon.hpp"
#include "ui/horizontal-loading-bar.hpp"
#include "ui/icon-button.hpp"
#include "ui/argument-completer-widget.hpp"
#include "ui/image/omnimg.hpp"
#include "ui/inline_qline_edit.hpp"
#include "ui/input_completer.hpp"
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qevent.h>
#include <qlayoutitem.h>
#include <qlineedit.h>
#include <qnamespace.h>
#include <qstackedwidget.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <qwidget.h>

struct TopBar : public QWidget {
  Q_OBJECT

public:
  IconButton *backButton = nullptr;
  QVBoxLayout *m_vlayout = new QVBoxLayout();
  QHBoxLayout *layout;
  SearchBar *input;
  HorizontalLoadingBar *m_loadingBar = new HorizontalLoadingBar;
  QWidget *m_backButtonSpacer = new QWidget(this);
  QWidget *m_accessory = new QWidget(this);
  QStackedWidget *m_accessoryContainer = new QStackedWidget(this);
  ArgumentCompleterWidget *m_completer = new ArgumentCompleterWidget(this);
  InputCompleter *quickInput = nullptr;
  std::optional<CompleterData> completerData;

public:
  TopBar(QWidget *parent = nullptr);

  void setAccessoryWidget(QWidget *widget);
  void clearAccessoryWidget();
  QWidget *accessoryWidget() const;

  bool eventFilter(QObject *obj, QEvent *event) override;
  void showBackButton();
  void setBackButtonVisiblity(bool value);
  void hideBackButton();

  void destroyCompleter() {
    m_completer->hide();
    input->setInline(false);
  }

  void setLoading(bool value);

  void activateCompleter(const CompleterData &data,
                         const std::vector<std::pair<QString, QString>> &values = {}) {
    while (m_completer->m_layout->count() > 1) {
      auto item = m_completer->m_layout->takeAt(1);
      if (auto w = item->widget()) w->deleteLater();
    }

    m_completer->m_icon->setUrl(data.iconUrl);
    m_completer->m_inputs.clear();
    m_completer->m_args.clear();
    m_completer->m_inputs.reserve(data.arguments.size());

    for (int i = 0; i != data.arguments.size(); ++i) {
      auto &arg = data.arguments.at(i);
      auto input = new InlineQLineEdit(arg.placeholder);

      if (i < values.size()) { input->setText(values.at(i).second); }

      connect(input, &InlineQLineEdit::textChanged, this, [this, input]() {
        input->clearError();
        emit argumentsChanged(m_completer->collect());
      });

      input->installEventFilter(this);

      m_completer->m_layout->addWidget(input);
      m_completer->m_inputs.emplace_back(input);
      m_completer->m_args.emplace_back(arg);
    }

    m_completer->m_layout->addStretch();

    auto fm = input->fontMetrics();

    input->setInline(true);
    m_completer->show();
  }

signals:
  void argumentsChanged(const std::vector<std::pair<QString, QString>> &values) const;
};
