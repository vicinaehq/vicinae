#include <QDateTime>
#include <QEvent>
#include <QGuiApplication>
#include <QInputMethod>
#include <QQuickWindow>
#include <QUuid>
#include "notes-window-controller.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/paste/paste-service.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/bridges/config-bridge.hpp"
#include "theme/theme.hpp"
#include "ui/quick/syntax-highlighter.hpp"
#include "ui/views/view-utils.hpp"
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
  connect(&m_save, &QFutureWatcher<Notes::Result<void>>::finished, this, &NotesWindowController::finishSave);
  connect(qApp, &QCoreApplication::aboutToQuit, this, [this] { flush(); });
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
    // QWindow's XCB backend reads this role when creating the native window.
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
  m_persisted = persisted;
  m_serial = m_savedSerial = 0;
  m_loading = false;
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
  if (m_note.title == title.toStdString()) return;
  m_note.title = title.toStdString();
  emit titleChanged();
  edited();
}

void NotesWindowController::edited() {
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
  if (!dirty() || m_saving || m_note.id.empty()) return;
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
    m_persisted = true;
    m_savedSerial = m_savingSerial;
    m_error.clear();
    if (dirty()) m_idleSave.start();
  } else {
    qWarning() << "Save note:" << result.error();
    m_error = tr("Could not save this note. Your changes are still here. Try saving again.");
  }
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
  if (m_saving) return tr("Saving…");
  if (dirty()) return tr("Unsaved changes");
  return m_persisted ? tr("Saved locally") : tr("New note");
}

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
