#pragma once

#include <QFutureWatcher>
#include <QPointer>
#include <QQmlApplicationEngine>
#include <QTimer>
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
  Q_PROPERTY(QString error READ error NOTIFY stateChanged)
  Q_PROPERTY(bool loading READ loading NOTIFY stateChanged)
  Q_PROPERTY(ViewMode viewMode READ viewMode WRITE setViewMode NOTIFY viewModeChanged)

public:
  Q_INVOKABLE void newNote();
  Q_INVOKABLE void search(QQuickItem *anchor = nullptr);
  Q_INVOKABLE void deleteCurrent();
  Q_INVOKABLE void save();
  Q_INVOKABLE bool requestClose();

signals:
  void titleChanged();
  void stateChanged();
  void viewModeChanged();
  void focusRequested();
  void closed();

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
  Vicinae::Editor::EditorDocument *document() { return &m_document; }
  MarkdownModel *previewModel() { return &m_previewModel; }
  QString title() const { return QString::fromStdString(m_note.title); }
  void setTitle(const QString &title);
  QString status() const;
  QString error() const { return m_error; }
  bool loading() const { return m_loading; }
  ViewMode viewMode() const { return m_viewMode; }
  void setViewMode(ViewMode mode);

private:
  bool eventFilter(QObject *object, QEvent *event) override;
  void ensureWindow();
  void activate(Notes::Note note, bool persisted);
  void edited();
  void refreshPreview();
  void finishSave();
  bool flush();
  Notes::Note snapshot() const;
  bool dirty() const { return m_serial != m_savedSerial; }
  ApplicationContext &m_context;
  Vicinae::Editor::EditorDocument m_document;
  MarkdownModel m_previewModel;
  Notes::Note m_note;
  QFutureWatcher<Notes::Result<void>> m_save;
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
