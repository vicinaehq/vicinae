#include <QDateTime>
#include <QClipboard>
#include <QEvent>
#include <QGuiApplication>
#include <QInputMethod>
#include <QMimeData>
#include <QQuickWindow>
#include <QUuid>
#include <utility>
#include "notes-window-controller.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/paste/paste-service.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/bridges/config-bridge.hpp"
#include "theme/theme.hpp"
#include "ui/quick/syntax-highlighter.hpp"
#include "ui/views/view-utils.hpp"
#include "ui/settings/settings-controller.hpp"
#include "ui/windows/launcher-window-platform.hpp"
#ifdef Q_OS_MACOS
#include "ui/quick/macos-chrome-attached.hpp"
#elif defined(Q_OS_LINUX)
#include "internal/wayland/xdg-activation.hpp"
#endif

NotesWindowController::NotesWindowController(ApplicationContext &context, std::string id, QObject *parent)
    : QObject(parent), m_context(context) {
  auto applyTheme = [this](const ThemeFile &theme) { m_document.setPalette(syntax::paletteForTheme(theme)); };
  applyTheme(ThemeService::instance().theme());
  connect(&ThemeService::instance(), &ThemeService::themeChanged, this, applyTheme);
  m_idleSave.setSingleShot(true);
  m_idleSave.setInterval(500);
  m_maxSave.setSingleShot(true);
  m_maxSave.setInterval(3000);
  m_previewUpdate.setSingleShot(true);
  m_previewUpdate.setInterval(0);
  connect(&m_previewUpdate, &QTimer::timeout, this, &NotesWindowController::refreshPreview);
  connect(&m_document, &Vicinae::Editor::EditorDocument::textChanged, this, [this] {
    if (m_viewMode == Split) m_previewUpdate.start();
  });
  connect(&m_document, &Vicinae::Editor::EditorDocument::textChanged, this, &NotesWindowController::edited);
  connect(&m_idleSave, &QTimer::timeout, this, &NotesWindowController::save);
  connect(&m_maxSave, &QTimer::timeout, this, &NotesWindowController::save);
  connect(&m_save, &QFutureWatcher<Notes::Result<Notes::Note>>::finished, this,
          &NotesWindowController::finishSave);
  connect(&m_attachmentSave, &QFutureWatcher<Notes::Result<Notes::AttachmentResult>>::finished, this,
          &NotesWindowController::finishPaste);
  connect(QGuiApplication::clipboard(), &QClipboard::dataChanged, this,
          &NotesWindowController::clipboardChanged);
  m_document.setDropHandler(
      [this](const QMimeData *mime) {
        return !m_loading && !m_closed && !readOnly() && !m_pasting &&
               (Notes::canImportAttachments(mime) || mime->hasText());
      },
      [this](const QMimeData *mime, int position) { return insertAttachments(mime, position, position); });
  connect(qApp, &QCoreApplication::aboutToQuit, this, [this] { flush(); });
  connect(m_context.services->notes(), &Notes::Store::changed, this, &NotesWindowController::refreshFromDisk);
  qApp->installEventFilter(this);
  const auto now = QDateTime::currentMSecsSinceEpoch();
  activate({std::move(id), {}, {}, now, now}, false);
}

NotesWindowController::~NotesWindowController() { flush(); }

bool NotesWindowController::eventFilter(QObject *object, QEvent *event) {
  if (object == qApp && event->type() == QEvent::Quit && !flush()) {
    event->ignore();
    present();
    return true;
  }
  return QObject::eventFilter(object, event);
}

void NotesWindowController::ensureWindow() {
  if (m_engine) return;
  m_engine = std::make_unique<QQmlApplicationEngine>();
  connect(m_engine.get(), &QQmlEngine::quit, qApp, &QCoreApplication::quit);
  QmlEngineScope::set(m_engine.get(), this);
  QmlEngineScope::set(m_engine.get(), new ConfigBridge(ConfigBridge::OpaqueSurfaces, this));
  const auto component =
#ifdef Q_OS_MACOS
      u"NotesWindowMacOS";
#elif defined(Q_OS_WIN)
      u"NotesWindowWindows";
#else
      u"NotesWindow";
#endif
  m_engine->load(qml::componentUrl(component));
  for (auto *root : m_engine->rootObjects())
    m_window = qobject_cast<QQuickWindow *>(root);
  if (m_window) {
    m_window->setProperty(PasteService::PASTE_TARGET_PROPERTY, true);
#ifdef Q_OS_LINUX
    m_window->setProperty("_q_xcb_wm_window_role", QByteArrayLiteral("vicinae-notes"));
#endif
  }
}

void NotesWindowController::present() {
  ensureWindow();
  if (!m_window) return;
  LauncherWindowPlatform::grantForeground();
  if (m_window->visibility() == QWindow::Minimized)
    m_window->showNormal();
  else
    m_window->show();
  m_window->raise();
  m_window->requestActivate();
#ifdef Q_OS_MACOS
  macosActivateApp();
#elif defined(Q_OS_LINUX)
  Wayland::XdgActivation::activateWindow(m_window);
#endif
  emit focusRequested();
}

void NotesWindowController::activate(Notes::Note note, bool persisted) {
  m_note = std::move(note);
  m_previewModel.setBaseUrl(QUrl::fromLocalFile(QString::fromStdString(m_note.path)));
  m_persisted = persisted;
  m_serial = m_savedSerial = 0;
  m_loading = false;
  m_attachmentError = false;
  m_error.clear();
  m_document.load(QString::fromStdString(m_note.id), QString::fromStdString(m_note.markdown));
  m_viewMode = Edit;
  m_previewUpdate.stop();
  emit titleChanged();
  emit viewModeChanged();
  emit stateChanged();
}

void NotesWindowController::newNote() { m_context.notes->newNote(); }

void NotesWindowController::loadNote() {
  const auto generation = ++m_loadGeneration;
  m_loading = true;
  emit stateChanged();
  present();
  m_context.services->notes()->load(m_note.id).then(
      this, [this, generation](Notes::Result<Notes::Note> result) {
        if (generation != m_loadGeneration) return;
        if (result)
          activate(std::move(*result), true);
        else {
          m_loading = false;
          m_error = tr("Could not open this note. Close this window and try again.");
          emit stateChanged();
        }
        emit focusRequested();
      });
}

void NotesWindowController::setTitle(const QString &title) {
  if (readOnly() || m_note.title == title.toStdString()) return;
  m_note.title = title.toStdString();
  emit titleChanged();
  edited();
}

void NotesWindowController::edited() {
  if (m_loading || readOnly()) return;
  if (m_attachmentError) {
    m_attachmentError = false;
    m_error.clear();
  }
  ++m_serial;
  m_idleSave.start();
  if (!m_maxSave.isActive()) m_maxSave.start();
  emit stateChanged();
}

Notes::Note NotesWindowController::snapshot() const {
  auto note = m_note;
  note.markdown = m_document.text().toStdString();
  note.updatedAt = QDateTime::currentMSecsSinceEpoch();
  return note;
}

void NotesWindowController::save() {
  if (!dirty() || m_saving || m_pasting || readOnly() || m_note.id.empty()) return;
  m_idleSave.stop();
  m_maxSave.stop();
  auto note = snapshot();
  if (!m_persisted && note.title.empty() && note.markdown.empty()) {
    m_savedSerial = m_serial;
    emit stateChanged();
    return;
  }
  m_saving = true;
  m_savingSerial = m_serial;
  m_save.setFuture(m_context.services->notes()->save(std::move(note), !m_persisted));
  emit stateChanged();
}

void NotesWindowController::finishSave() {
  if (!m_saving || !m_save.isFinished()) return;
  m_saving = false;
  const auto result = m_save.result();
  if (result) {
    applySavedNote(*result, m_savingSerial);
    if (!m_attachmentError) m_error.clear();
    if (dirty()) m_idleSave.start();
    if (m_pasting) startPaste();
  } else {
    m_pasting = false;
    m_attachmentError = false;
    m_pendingAttachments.clear();
    qWarning() << "Save note:" << result.error();
    m_error = tr("Could not save this note. %1").arg(QString::fromStdString(result.error()));
  }
  emit stateChanged();
}

void NotesWindowController::applySavedNote(const Notes::Note &note, std::uint64_t serial) {
  m_persisted = true;
  m_note.path = note.path;
  m_note.revision = note.revision;
  m_note.updatedAt = note.updatedAt;
  m_previewModel.setBaseUrl(QUrl::fromLocalFile(QString::fromStdString(note.path)));
  if (m_serial == serial) {
    m_note.title = note.title;
    emit titleChanged();
  }
  m_savedSerial = serial;
}

bool NotesWindowController::canPasteAttachments() const {
  return Notes::canImportAttachments(QGuiApplication::clipboard()->mimeData());
}

bool NotesWindowController::paste(int start, int end) {
  return insertAttachments(QGuiApplication::clipboard()->mimeData(), start, end);
}

bool NotesWindowController::insertAttachments(const QMimeData *mime, int start, int end) {
  if (m_loading || m_closed || readOnly()) return false;
  if (!Notes::canImportAttachments(mime)) return false;
  if (m_pasting) return true;
  auto *quickDocument = m_document.textDocument();
  if (!quickDocument || start < 0 || end < start || end >= quickDocument->textDocument()->characterCount())
    return true;
  m_pendingAttachments = Notes::attachmentsFromMimeData(mime);
  if (m_pendingAttachments.empty()) return false;
  m_pasteCursor = QTextCursor(quickDocument->textDocument());
  m_pasteCursor.setPosition(start);
  m_pasteCursor.setPosition(end, QTextCursor::KeepAnchor);
  m_pasteCursor.setKeepPositionOnInsert(true);
  m_pasteSelection = m_pasteCursor.selectedText();
  m_pasting = true;
  m_attachmentError = false;
  m_error.clear();
  if (!m_saving) startPaste();
  emit stateChanged();
  return true;
}

void NotesWindowController::startPaste() {
  m_idleSave.stop();
  m_maxSave.stop();
  m_attachmentSerial = m_serial;
  m_attachmentSave.setFuture(
      m_context.services->notes()->attach(snapshot(), !m_persisted, std::exchange(m_pendingAttachments, {})));
}

void NotesWindowController::finishPaste() {
  if (!m_pasting || !m_pendingAttachments.empty() || !m_attachmentSave.isFinished()) return;
  m_pasting = false;
  const auto result = m_attachmentSave.result();
  // Qt can move the anchor when typing at a cursor whose position is held fixed.
  if (m_pasteSelection.isEmpty()) m_pasteCursor.clearSelection();
  if (!result) {
    m_error = tr("Could not add the attachment. %1").arg(QString::fromStdString(result.error()));
  } else {
    applySavedNote(result->note, m_attachmentSerial);
    if (!result->markdown) {
      m_attachmentError = true;
      m_error = tr("Could not add the attachment. %1").arg(QString::fromStdString(result->markdown.error()));
    } else if (m_pasteCursor.selectedText() != m_pasteSelection) {
      m_attachmentError = true;
      m_error = tr("The selected text changed while adding the attachment. Try adding it again.");
    } else {
      auto *item = m_document.textDocument()->parent();
      const bool restoreCursor = item->property("selectionStart").toInt() == m_pasteCursor.selectionStart() &&
                                 item->property("selectionEnd").toInt() == m_pasteCursor.selectionEnd();
      m_pasteCursor.setKeepPositionOnInsert(false);
      m_pasteCursor.beginEditBlock();
      m_pasteCursor.insertText(QString::fromStdString(*result->markdown));
      m_pasteCursor.endEditBlock();
      if (restoreCursor)
        emit m_document.selectionRequested(m_pasteCursor.position(), m_pasteCursor.position());
      m_error.clear();
    }
  }
  if (dirty()) m_idleSave.start();
  emit stateChanged();
}

bool NotesWindowController::flush() {
  if (m_closed) return true;
  QGuiApplication::inputMethod()->commit();
  m_idleSave.stop();
  m_maxSave.stop();
  if (m_saving) {
    m_save.waitForFinished();
    finishSave();
    if (!m_save.result()) return false;
  }
  if (m_pasting) {
    m_attachmentSave.waitForFinished();
    finishPaste();
    if (!m_error.isEmpty()) return false;
  }
  save();
  if (m_saving) {
    m_save.waitForFinished();
    finishSave();
  }
  return !dirty();
}

QString NotesWindowController::status() const {
  if (m_loading) return tr("Opening…");
  if (m_pasting) return tr("Adding attachment…");
  if (m_saving) return tr("Saving…");
  if (dirty()) return tr("Unsaved changes");
  if (readOnly()) return tr("Read-only");
  return m_persisted ? tr("Saved locally") : tr("New note");
}

void NotesWindowController::refreshFromDisk() {
  if (!m_persisted || m_closed || m_loading || m_saving || m_pasting || dirty()) return;
  const auto serial = m_serial;
  const auto generation = ++m_loadGeneration;
  m_context.services->notes()->load(m_note.id).then(
      this, [this, serial, generation](Notes::Result<Notes::Note> result) {
        if (m_closed || m_loading || m_saving || m_pasting || dirty() || serial != m_serial ||
            generation != m_loadGeneration)
          return;
        if (!result) {
          m_attachmentError = false;
          m_error = tr("The file is unavailable. Save a copy to keep this note.");
          emit stateChanged();
          return;
        }
        if (result->revision == m_note.revision && result->title == m_note.title) {
          m_note.readOnly = result->readOnly;
          if (!m_attachmentError) m_error.clear();
          emit stateChanged();
          return;
        }
        const auto mode = m_viewMode;
        activate(std::move(*result), true);
        setViewMode(mode);
      });
}

void NotesWindowController::reload() {
  if (m_pasting && !flush()) return;
  m_idleSave.stop();
  m_maxSave.stop();
  if (m_saving) {
    m_save.waitForFinished();
    finishSave();
  }
  const auto generation = ++m_loadGeneration;
  m_loading = true;
  emit stateChanged();
  m_context.services->notes()->load(m_note.id).then(
      this, [this, generation](Notes::Result<Notes::Note> result) {
        if (m_closed || generation != m_loadGeneration) return;
        m_loading = false;
        if (result)
          activate(std::move(*result), true);
        else {
          m_error = tr("Could not reload this note. Your changes are still here.");
          emit stateChanged();
        }
      });
}

void NotesWindowController::saveCopy() {
  if (m_pasting && !flush()) return;
  m_idleSave.stop();
  m_maxSave.stop();
  if (m_saving) {
    m_save.waitForFinished();
    finishSave();
  }
  auto copy = snapshot();
  copy.id = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
  auto future = m_context.services->notes()->save(std::move(copy), true);
  future.waitForFinished();
  const auto result = future.result();
  if (!result) {
    m_error = tr("Could not save a copy. %1").arg(QString::fromStdString(result.error()));
    emit stateChanged();
    return;
  }
  m_context.notes->openNote(QString::fromStdString(result->id));
  m_savedSerial = m_serial;
  m_closed = true;
  if (m_window) m_window->close();
  emit closed();
}

void NotesWindowController::openSettings() { m_context.settings->openExtensionPreferences({"notes", {}}); }

bool NotesWindowController::requestClose() {
  if (m_closed) return true;
  ++m_loadGeneration;
  m_loading = false;
  if (!flush()) return false;
  m_closed = true;
  emit closed();
  return true;
}

void NotesWindowController::setViewMode(ViewMode mode) {
  if (m_viewMode == mode) return;
  QGuiApplication::inputMethod()->commit();
  m_viewMode = mode;
  m_previewUpdate.stop();
  if (mode != Edit) refreshPreview();
  emit viewModeChanged();
  if (mode != Preview) emit focusRequested();
}

void NotesWindowController::refreshPreview() {
  if (m_closed || m_viewMode == Edit) return;
  auto markdown = m_document.text();
  if (markdown != m_previewModel.markdown()) m_previewModel.setMarkdownAsync(std::move(markdown), true);
}

void NotesWindowController::search(QQuickItem *anchor) {
  if (!flush()) return;
  m_context.navigation->activateEntrypoint({"notes", "search"}, {.toggleAnchor = anchor});
}

void NotesWindowController::deleteCurrent() {
  if (readOnly()) return;
  if (m_pasting && !flush()) return;
  if (!m_persisted) {
    m_closed = true;
    if (m_window) m_window->close();
    emit closed();
    return;
  }
  ++m_loadGeneration;
  m_loading = false;
  emit stateChanged();
  m_idleSave.stop();
  m_maxSave.stop();
  if (m_saving) {
    m_save.waitForFinished();
    finishSave();
  }
  auto future = m_context.services->notes()->remove(m_note.id);
  future.waitForFinished();
  if (!future.result()) {
    m_error = tr("Could not delete this note. Try again.");
    emit stateChanged();
    m_context.services->toastService()->failure(tr("Could not delete note."));
    return;
  }
  m_closed = true;
  if (m_window) m_window->close();
  emit closed();
}

NotesWindowController *NotesWindowManager::createWindow(std::string id) {
  auto *controller = new NotesWindowController(m_context, id, this);
  m_windows.emplace(id, controller);
  connect(controller, &NotesWindowController::closed, this, [this, id = std::move(id), controller] {
    m_windows.erase(id);
    controller->deleteLater();
  });
  return controller;
}

void NotesWindowManager::newNote() {
  createWindow(QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString())->present();
}

void NotesWindowManager::openNote(const QString &id) {
  const auto key = id.toStdString();
  if (const auto it = m_windows.find(key); it != m_windows.end()) {
    it->second->present();
    return;
  }
  createWindow(key)->loadNote();
}

void NotesWindowManager::deleteNote(const QString &id) {
  const auto key = id.toStdString();
  if (const auto it = m_windows.find(key); it != m_windows.end()) {
    it->second->deleteCurrent();
    return;
  }
  auto future = m_context.services->notes()->remove(key);
  future.waitForFinished();
  if (!future.result()) m_context.services->toastService()->failure(tr("Could not delete note."));
}
