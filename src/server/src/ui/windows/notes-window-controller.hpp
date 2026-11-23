#pragma once

#include <QFutureWatcher>
#include <QPointer>
#include <QQmlApplicationEngine>
#include <QTimer>
#include <QTextCursor>
#include <unordered_map>
#include "common/context.hpp"
#include "editor-document.hpp"
#include "services/notes/notes-store.hpp"
#include "ui/qml-engine-scope.hpp"
#include "ui/quick/markdown-model.hpp"

class QQuickWindow;

class NotesWindowController : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(NotesWindowHost)
  QML_SINGLETON
  Q_PROPERTY(Vicinae::Editor::EditorDocument *document READ document CONSTANT)
  Q_PROPERTY(MarkdownModel *previewModel READ previewModel CONSTANT)
  Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
  Q_PROPERTY(QString status READ status NOTIFY stateChanged)
  Q_PROPERTY(bool readOnly READ readOnly NOTIFY stateChanged)
  Q_PROPERTY(QString error READ error NOTIFY stateChanged)
  Q_PROPERTY(bool loading READ loading NOTIFY stateChanged)
  Q_PROPERTY(bool canPasteAttachments READ canPasteAttachments NOTIFY clipboardChanged)
  Q_PROPERTY(ViewMode viewMode READ viewMode WRITE setViewMode NOTIFY viewModeChanged)
  Q_PROPERTY(qreal fontSize READ fontSize NOTIFY fontSizeChanged)

public:
  Q_INVOKABLE void newNote();
  Q_INVOKABLE void search(QQuickItem *anchor = nullptr);
  Q_INVOKABLE void deleteCurrent();
  Q_INVOKABLE void save();
  Q_INVOKABLE void saveCopy();
  Q_INVOKABLE void reload();
  Q_INVOKABLE void openSettings();
  Q_INVOKABLE bool requestClose();
  Q_INVOKABLE bool paste(int start, int end);
  Q_INVOKABLE void adjustFontSize(int delta);
  Q_INVOKABLE void resetFontSize();

signals:
  void titleChanged();
  void stateChanged();
  void viewModeChanged();
  void fontSizeChanged();
  void focusRequested();
  void closed();
  void clipboardChanged();

public:
  enum ViewMode { Edit, Split, Preview };
  Q_ENUM(ViewMode)

  NotesWindowController(ApplicationContext &context, std::string id, QObject *parent);
  ~NotesWindowController() override;
  static NotesWindowController *create(QQmlEngine *engine, QJSEngine *) {
    return QmlEngineScope::get<NotesWindowController>(engine);
  }
  void loadNote();
  void present();
  bool isEmptyDraft() const;
  Vicinae::Editor::EditorDocument *document() { return &m_document; }
  MarkdownModel *previewModel() { return &m_previewModel; }
  QString title() const { return QString::fromStdString(m_note.title); }
  void setTitle(const QString &title);
  QString status() const;
  bool readOnly() const { return m_note.readOnly; }
  QString error() const { return m_error; }
  bool loading() const { return m_loading; }
  bool canPasteAttachments() const;
  ViewMode viewMode() const { return m_viewMode; }
  void setViewMode(ViewMode mode);
  qreal fontSize() const;

private:
  bool eventFilter(QObject *object, QEvent *event) override;
  void ensureWindow();
  void activate(Notes::Note note, bool persisted);
  void edited();
  void refreshFromDisk();
  void refreshPreview();
  void finishSave();
  void applySavedNote(const Notes::Note &note, std::uint64_t serial);
  bool insertAttachments(const QMimeData *mime, int start, int end);
  void startPaste();
  void finishPaste();
  bool flush();
  void storeFontSize(std::optional<int> size);
  Notes::Note snapshot() const;
  bool dirty() const { return m_serial != m_savedSerial; }
  ApplicationContext &m_context;
  Vicinae::Editor::EditorDocument m_document;
  MarkdownModel m_previewModel;
  Notes::Note m_note;
  QFutureWatcher<Notes::Result<Notes::Note>> m_save;
  QFutureWatcher<Notes::Result<Notes::AttachmentResult>> m_attachmentSave;
  std::vector<Notes::Attachment> m_pendingAttachments;
  QTextCursor m_pasteCursor;
  QString m_pasteSelection;
  std::uint64_t m_attachmentSerial = 0;
  bool m_pasting = false;
  bool m_attachmentError = false;
  bool m_crlf = false;
  QTimer m_idleSave;
  QTimer m_maxSave;
  QTimer m_previewUpdate;
  QString m_error;
  std::uint64_t m_serial = 0;
  std::uint64_t m_savedSerial = 0;
  std::uint64_t m_savingSerial = 0;
  std::uint64_t m_loadGeneration = 0;
  bool m_saving = false;
  bool m_persisted = false;
  bool m_loading = false;
  ViewMode m_viewMode = Edit;
  bool m_closed = false;
  QPointer<QQuickWindow> m_window;
  std::unique_ptr<QQmlApplicationEngine> m_engine;
};

class NotesWindowManager : public QObject {
  Q_OBJECT

public:
  explicit NotesWindowManager(ApplicationContext &context) : m_context(context) {}
  void newNote();
  void openNote(const QString &id);
  void deleteNote(const QString &id);

private:
  NotesWindowController *createWindow(std::string id);
  ApplicationContext &m_context;
  std::unordered_map<std::string, NotesWindowController *> m_windows;
};
