#pragma once

#include <QObject>
#include <QTimer>
#include <unordered_set>
#include <xcb/xcb.h>

class QSocketNotifier;

/**
 * Lightweight watcher for X11/XCB window-manager events. Owns its own XCB connection
 * so that we only receive root-window notifications instead of the whole Qt event stream.
 */
class X11EventListener : public QObject {
  Q_OBJECT

public:
  explicit X11EventListener(QObject *parent = nullptr);
  ~X11EventListener() override;

  /**
   * Start listening for root-window property/substructure notifications.
   * Returns true on success, false if the connection or subscription failed.
   */
  bool start();

signals:
  void windowListChanged() const;
  void activeWindowChanged() const;
  void windowTitleChanged(xcb_window_t window) const;

private slots:
  void drainEvents();
  void emitDebouncedSignals();

private:
  void subscribeToRootEvents();
  xcb_atom_t internAtom(const char *name) const;
  std::unordered_set<xcb_window_t> fetchClientList() const;
  xcb_window_t fetchActiveWindow() const;

  xcb_connection_t *m_connection = nullptr;
  xcb_screen_t *m_screen = nullptr;
  xcb_window_t m_root = XCB_WINDOW_NONE;
  QSocketNotifier *m_notifier = nullptr;

  xcb_atom_t m_atomClientList = XCB_ATOM_NONE;
  xcb_atom_t m_atomActiveWindow = XCB_ATOM_NONE;
  xcb_atom_t m_atomWmName = XCB_ATOM_NONE;

  std::unordered_set<xcb_window_t> m_clients;
  xcb_window_t m_activeWindow = XCB_WINDOW_NONE;

  // Debouncing
  QTimer m_debounceTimer;
  bool m_pendingWindowListChanged = false;
  bool m_pendingActiveWindowChanged = false;
  std::unordered_set<xcb_window_t> m_pendingTitlesChanged;
};

