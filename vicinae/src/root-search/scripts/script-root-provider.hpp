#include "actions/files/file-actions.hpp"
#include "argument.hpp"
#include "common.hpp"
#include "layout.hpp"
#include "navigation-controller.hpp"
#include "script-command.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "services/script-command/script-command-service.hpp"
#include "theme.hpp"
#include "theme/colors.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/scroll-bar/scroll-bar.hpp"
#include "ui/views/base-view.hpp"
#include "utils.hpp"
#include "services/app-service/app-service.hpp"
#include "services/toast/toast-service.hpp"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <qnamespace.h>
#include <qplaintextedit.h>
#include <qprocess.h>
#include <qstringliteral.h>
#include <qstringview.h>
#include <qtextbrowser.h>
#include <qtextdocument.h>
#include <qtextedit.h>
#include <qtextformat.h>
#include <ranges>

class ScriptOutputTokenizer {
public:
  struct Format {
    bool underline = false;
    bool reset = false;
    bool italic = false;
    std::optional<QColor> fg;
    std::optional<QColor> bg;
  };

  struct Token {
    QString text;
    bool url = false;
    std::optional<Format> fmt;
  };

  enum State : std::uint8_t { Normal, Escape, Color, Url };

  ScriptOutputTokenizer(QStringView str) : m_data(str) {}

  static ColorLike getStandardColor(std::uint8_t code) {
    switch (code) {
    case 0:
      return QColor(Qt::transparent);
    case 30:
      return QColor(Qt::black);
    case 31:
      return SemanticColor::Red;
    case 32:
      return SemanticColor::Green;
    case 33:
      return SemanticColor::Yellow;
    case 34:
      return SemanticColor::Blue;
    case 35:
      return SemanticColor::Magenta;
    case 36:
      return SemanticColor::Cyan;
    case 97:
      return QColor(Qt::white);
    default:
      return SemanticColor::TextPrimary;
    }
  }

  static QColor parseFgColor(std::uint8_t code) { return OmniPainter::resolveColor(getStandardColor(code)); }

  static bool isValidUrlChar(QChar c) { return c.isPrint() && !c.isSpace() && c != '"' && c != '\''; }

  static QColor parseBgColor(int code) { return OmniPainter::resolveColor(getStandardColor(code - 10)); }

  static Format parseColor(const std::vector<std::uint8_t> &codes) {
    Format fmt;

    if (codes.size() == 2) {
      fmt.fg = parseFgColor(codes.at(0));
      fmt.bg = parseBgColor(codes.at(1));
    }

    if (codes.size() == 1) {
      if (codes.at(0) == 0) {
        fmt.reset = true;
      } else {
        fmt.fg = parseFgColor(codes.at(0));
      }
    }

    return fmt;
  }

  std::optional<Token> next() {
    static const auto urlSchemes = {QStringLiteral("http://"), QStringLiteral("https://")};

    if (m_cursor >= m_data.size()) return {};

    size_t beginIdx = m_cursor;
    Token tok;

    std::vector<uint8_t> colorCodes;

    while (m_cursor < m_data.size()) {
      QChar ch = m_data.at(m_cursor);
      QStringView rem = m_data.sliced(m_cursor);

      switch (m_state) {
      case State::Normal: {
        if (ch == '\033') {
          if (tok.fmt) return tok;
          m_state = State::Escape;
        } else {
          bool maybeUrl = std::ranges::any_of(urlSchemes, [&](auto s) { return rem.startsWith(s); });

          if (maybeUrl) {
            m_state = Url;
            return tok;
          }

          tok.text.append(ch);
        }
        break;
      }
      case State::Url: {
        if (isValidUrlChar(ch)) {
          tok.text.append(ch);
        } else {
          m_state = State::Normal;
          tok.url = true;
          return tok;
        }
        break;
      }
      case State::Escape: {
        if (ch == '[') {
          if (!tok.text.isEmpty()) { return tok; }
          colorCodes.clear();
          colorCodes.emplace_back(0);
          m_state = State::Color;
        }
        break;
      }
      case State::Color: {
        if (ch.isNumber()) {
          char n = ch.toLatin1() - 48;
          colorCodes.back() = colorCodes.back() * 10 + n;
        } else if (ch == ';') {
          colorCodes.emplace_back(0);
        } else if (ch == 'm') {
          tok.fmt = parseColor(colorCodes);
          m_state = Normal;
        }
        break;
      }
      }

      ++m_cursor;
    }

    return tok;
  }

private:
  QStringView m_data;
  size_t m_cursor = 0;
  State m_state = Normal;
};

class ScriptOutputTextDocument : public QTextDocument {
public:
  ScriptOutputTextDocument() = default;

  void setScriptText(const QString &text) {
    QTextCursor cursor(this);
    QTextBlockFormat bfmt = cursor.blockFormat();
    QTextCharFormat defaultCharFmt = cursor.charFormat();

    cursor.movePosition(QTextCursor::MoveOperation::StartOfBlock);

    defaultCharFmt.setFontPointSize(10.5);
    cursor.setCharFormat(defaultCharFmt);

    ScriptOutputTokenizer tokenizer(text);

    while (const auto tok = tokenizer.next()) {
      if (const auto &fmt = tok->fmt) {
        QTextCharFormat newFormat = cursor.charFormat();

        newFormat.setFontUnderline(fmt->underline);
        newFormat.setFontItalic(fmt->italic);

        if (fmt->reset) newFormat = defaultCharFmt;
        if (fmt->fg) { newFormat.setForeground(*fmt->fg); }
        if (fmt->bg) { newFormat.setBackground(*fmt->bg); }

        cursor.setCharFormat(newFormat);
      }

      QTextCharFormat old = cursor.charFormat();

      if (tok->url && QUrl(tok->text).isValid()) {
        QTextCharFormat linkFormat;
        linkFormat.setAnchor(true);
        linkFormat.setAnchorHref(tok->text);
        linkFormat.setForeground(OmniPainter::resolveColor(SemanticColor::LinkDefault));
        linkFormat.clearBackground();
        cursor.setCharFormat(linkFormat);
      }

      cursor.insertText(tok->text);
      cursor.setCharFormat(old);
    }
  }

private:
};

class ScriptExecutorView : public BaseView {
public:
  ScriptExecutorView(std::vector<std::string> cmdline) : m_cmdline(cmdline) {
    m_textEdit->setReadOnly(true);
    m_textEdit->setVerticalScrollBar(new OmniScrollBar);
    m_textEdit->setOpenExternalLinks(true);
    VStack().add(m_textEdit, 1).spacing(0).imbue(this);
  }

  virtual bool supportsSearch() const override { return false; }

  void beforePop() override {
    if (!m_exited && m_process.state() == QProcess::Running) { m_process.kill(); }
    context()->services->toastService()->failure("Script interrupted");
  }

  void initialize() override {
    auto ss = Utils::toQStringVec(m_cmdline);
    auto env = QProcessEnvironment::systemEnvironment();
    assert(ss.size() != 0);
    m_process.setProgram(ss.at(0));
    m_process.setArguments(ss | std::views::drop(1) | std::ranges::to<QList>());
    env.insert("FORCE_COLOR", "1");
    m_process.setEnvironment(env.toStringList());

    context()->services->toastService()->dynamic("Script is running...");

    connect(&m_process, &QProcess::readyReadStandardOutput, this, [this]() {
      bool isBottomScrolled =
          m_textEdit->verticalScrollBar()->value() == m_textEdit->verticalScrollBar()->maximum();

      buf.append(m_process.readAllStandardOutput());
      auto doc = new ScriptOutputTextDocument;
      QTextCursor cursor = m_textEdit->textCursor();
      int selectionStart = cursor.selectionStart();
      int selectionEnd = cursor.selectionEnd();

      int scrollHeight = m_textEdit->verticalScrollBar()->value();

      doc->setDocumentMargin(15);
      doc->setScriptText(buf);
      m_textEdit->setDocument(doc);
      m_textEdit->verticalScrollBar()->setValue(scrollHeight);

      QTextCursor newCursor = m_textEdit->textCursor();

      newCursor.setPosition(selectionStart);
      newCursor.setPosition(selectionEnd, QTextCursor::KeepAnchor);
      m_textEdit->setTextCursor(newCursor);

      if (isBottomScrolled) {
        m_textEdit->verticalScrollBar()->setValue(m_textEdit->verticalScrollBar()->maximum());
      } else {
        m_textEdit->verticalScrollBar()->setValue(scrollHeight);
      }

      if (m_doc) m_doc->deleteLater();
      m_doc = doc;
    });

    connect(&m_process, &QProcess::finished, this, [this](int code) {
      const auto msg = QString("Script exited with code %1").arg(code);
      context()->services->toastService()->success(msg);
    });

    m_process.start();
  }

private:
  QTextBrowser *m_textEdit = new QTextBrowser;
  ScriptOutputTextDocument *m_doc = nullptr;

  std::vector<std::string> m_cmdline;
  QProcess m_process;
  QString buf;
  bool m_exited = false;
};

class ScriptExecutorAction : public AbstractAction {
public:
  ScriptExecutorAction(const std::filesystem::path &path,
                       std::optional<script_command::OutputMode> mode = std::nullopt)
      : m_path(path), m_outputModeOverride(mode) {}

  void execute(ApplicationContext *ctx) override {
    const auto result = ScriptCommandFile::fromFile(m_path);

    if (!result) {
      ctx->services->toastService()->failure("Failed to parse script");
      return;
    }

    auto cmdline = result->data().exec;

    cmdline.emplace_back(m_path);

    for (const auto &value : ctx->navigation->completionValues()) {
      cmdline.emplace_back(value.second.toStdString());
    }

    using Mode = script_command::OutputMode;

    switch (m_outputModeOverride.value_or(result->data().mode)) {
    case Mode::Full:
      ctx->navigation->pushView(new ScriptExecutorView(cmdline));
      break;
    case Mode::Silent:
      ctx->services->appDb()->launchTerminalCommand(Utils::toQStringVec(cmdline), {.hold = true});
      break;
    case Mode::Compact:
      break;
    case Mode::Inline:
      break;
    }
  }

private:
  std::filesystem::path m_path;
  std::optional<script_command::OutputMode> m_outputModeOverride;
};

class ScriptRootItem : public RootItem {
  QString displayName() const override { return m_file.data().title.c_str(); }

  QString subtitle() const override { return m_file.packageName().c_str(); }

  QString typeDisplayName() const override { return "Script"; }

  ArgumentList arguments() const override {
    ArgumentList args;
    args.reserve(m_file.data().arguments.size());

    for (const auto &[idx, arg] : m_file.data().arguments | std::views::enumerate) {
      CommandArgument cmdArg;

      cmdArg.required = !arg.optional;
      cmdArg.name = QString("argument%1").arg(idx);
      cmdArg.placeholder = arg.placeholder.c_str();

      using T = script_command::ArgumentType;

      switch (arg.type) {
      case T::Text:
        cmdArg.type = CommandArgument::Text;
        break;
      case T::Password:
        cmdArg.type = CommandArgument::Password;
        break;
      case T::Dropdown:
        cmdArg.type = CommandArgument::Dropdown;
        break;
      }

      args.emplace_back(cmdArg);
    }

    return args;
  }

  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx,
                                                   const RootItemMetadata &metadata) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto section = panel->createSection();
    auto editor = ctx->services->appDb()->textEditor();

    section->addAction(new ScriptExecutorAction(m_file.path()));

    if (editor) { section->addAction(new OpenFileAction(m_file.path(), editor)); }

    return panel;
  }

  AccessoryList accessories() const override { return {{.text = "Script"}}; }

  EntrypointId uniqueId() const override { return EntrypointId("scripts", m_file.path()); };

  ImageURL iconUrl() const override { return m_file.icon(); }

  std::vector<QString> keywords() const override { return Utils::toQStringVec(m_file.data().keywords); }

public:
  ScriptRootItem(ScriptCommandFile &&file) : m_file(std::move(file)) {}

private:
  ScriptCommandFile m_file;
};

class ScriptRootProvider : public RootProvider {
public:
  std::vector<std::shared_ptr<RootItem>> loadItems() const override {
    return m_service.scanAll() | std::views::transform([](auto item) -> std::shared_ptr<RootItem> {
             return std::make_shared<ScriptRootItem>(std::move(item));
           }) |
           std::ranges::to<std::vector>();
  }

  Type type() const override { return Type::GroupProvider; }

  ImageURL icon() const override { return ImageURL::emoji("🤖"); }

  QString displayName() const override { return "Script Commands"; }

  QString uniqueId() const override { return "scripts"; }

  PreferenceList preferences() const override { return {}; }

  void preferencesChanged(const QJsonObject &preferences) override {}

  ScriptRootProvider() = default;

private:
  ScriptCommandService m_service;
};
