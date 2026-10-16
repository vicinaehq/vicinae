#include <catch2/catch_test_macros.hpp>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QCoreApplication>
#include <QEventLoop>
#include <QObject>
#include <QTimer>
#include <string>
#include <string_view>
#include "config/config.hpp"
#include "services/tray-host/sni/sni-watcher.hpp"

// NOLINTBEGIN(bugprone-throwing-static-initialization)

namespace {

constexpr int kBeyondGraceMs = 4000;

const QString &watcherService() {
  static const QString name = QStringLiteral("org.kde.StatusNotifierWatcher");
  return name;
}

const QString &watcherPath() {
  static const QString path = QStringLiteral("/StatusNotifierWatcher");
  return path;
}

// dbus-run-session points the process at a private bus. An empty address lets Qt
// autolaunch, and $XDG_RUNTIME_DIR/bus is the login session: never claim that name.
bool privateSessionBus() {
  const auto addr = qgetenv("DBUS_SESSION_BUS_ADDRESS");
  if (addr.isEmpty()) return false;

  const auto runtime = qgetenv("XDG_RUNTIME_DIR");
  if (!runtime.isEmpty() && addr.contains(runtime + "/bus")) return false;
  if (addr.contains("/run/user/") && addr.contains("/bus")) return false;
  return true;
}

void requirePrivateSessionBus() {
  if (!privateSessionBus()) {
    SKIP("SniWatcher bus cases require dbus-run-session and must not use the login session bus");
  }
}

void ensureCoreApp() {
  if (QCoreApplication::instance()) return;

  static int argc = 1;
  static char arg0[] = "vicinae-server-tests";
  static char *argv[] = {arg0, nullptr};
  static QCoreApplication app(argc, argv);
  Q_UNUSED(app);
}

void waitMs(int ms) {
  QEventLoop loop;
  QTimer::singleShot(ms, &loop, &QEventLoop::quit);
  loop.exec();
}

bool watcherServiceRegistered() {
  auto *iface = QDBusConnection::sessionBus().interface();
  if (!iface) return false;
  const QDBusReply<bool> reply = iface->isServiceRegistered(watcherService());
  return reply.isValid() && reply.value();
}

QString watcherOwner() {
  auto *iface = QDBusConnection::sessionBus().interface();
  if (!iface) return {};
  const QDBusReply<QString> reply = iface->serviceOwner(watcherService());
  if (!reply.isValid()) return {};
  return reply.value();
}

QObject *localWatcherObject() { return QDBusConnection::sessionBus().objectRegisteredAt(watcherPath()); }

// Separate connection so the test can own the watcher name without using the
// session connection SniWatcher claims on.
struct ExternalWatcher {
  QString name;
  QDBusConnection conn;
  QObject object;

  ExternalWatcher()
      : name(QStringLiteral("vicinae-sni-watcher-test-%1").arg(++sequence())),
        conn(QDBusConnection::connectToBus(QDBusConnection::SessionBus, name)) {}

  ExternalWatcher(const ExternalWatcher &) = delete;
  ExternalWatcher &operator=(const ExternalWatcher &) = delete;

  ~ExternalWatcher() {
    if (conn.isConnected()) {
      conn.unregisterService(watcherService());
      conn.unregisterObject(watcherPath());
    }
    QDBusConnection::disconnectFromBus(name);
  }

  const QDBusConnection &connection() const { return conn; }

  bool acquire() {
    if (!conn.isConnected() || !conn.interface()) return false;
    if (!conn.registerObject(watcherPath(), &object)) return false;

    const QDBusReply<QDBusConnectionInterface::RegisterServiceReply> reply =
        conn.interface()->registerService(watcherService(), QDBusConnectionInterface::DontQueueService,
                                          QDBusConnectionInterface::DontAllowReplacement);
    return reply.isValid() && reply.value() == QDBusConnectionInterface::ServiceRegistered;
  }

private:
  static int &sequence() {
    static int seq = 0;
    return seq;
  }
};

template <typename T> T readJson(std::string_view json) {
  T value{};
  const auto error = glz::read_json(value, json);
  if (error) { FAIL(glz::format_error(error, json)); }
  return value;
}

template <typename T, typename Base, typename Patch> T mergeJson(const Base &base, const Patch &patch) {
  std::string buf;
  const auto writeError = glz::write_json(glz::merge{base, patch}, buf);
  if (writeError) { FAIL(glz::format_error(writeError)); }

  T value{};
  const auto readError = glz::read_json(value, buf);
  if (readError) { FAIL(glz::format_error(readError, buf)); }
  return value;
}

} // namespace

TEST_CASE("tray watcher_enabled defaults to true and stays independent of enabled") {
  const auto iconOnly = readJson<config::Tray>(R"({"enabled":false})");
  CHECK_FALSE(iconOnly.enabled);
  CHECK(iconOnly.watcherEnabled);

  const auto watcherOff = readJson<config::Tray>(R"({"watcher_enabled":false})");
  CHECK(watcherOff.enabled);
  CHECK_FALSE(watcherOff.watcherEnabled);

  const auto omitted = readJson<config::Partial<config::Tray>>("{}");
  CHECK_FALSE(omitted.enabled.has_value());
  CHECK_FALSE(omitted.watcherEnabled.has_value());
  const auto omittedMerged = mergeJson<config::Tray>(config::Tray{}, omitted);
  CHECK(omittedMerged.enabled);
  CHECK(omittedMerged.watcherEnabled);

  const auto partialOff = readJson<config::Partial<config::Tray>>(R"({"watcher_enabled":false})");
  REQUIRE(partialOff.watcherEnabled.has_value());
  CHECK_FALSE(*partialOff.watcherEnabled);
  CHECK_FALSE(partialOff.enabled.has_value());
  const auto partialMerged = mergeJson<config::Tray>(config::Tray{}, partialOff);
  CHECK(partialMerged.enabled);
  CHECK_FALSE(partialMerged.watcherEnabled);

  const auto enabledOff = readJson<config::Partial<config::Tray>>(R"({"enabled":false})");
  const config::Tray watcherAlreadyOff{.enabled = true, .watcherEnabled = false};
  const auto enabledMerged = mergeJson<config::Tray>(watcherAlreadyOff, enabledOff);
  CHECK_FALSE(enabledMerged.enabled);
  CHECK_FALSE(enabledMerged.watcherEnabled);
}

TEST_CASE("disabled SNI watcher does not claim the name after the grace period") {
  requirePrivateSessionBus();
  ensureCoreApp();

  SniWatcher watcher(false);
  CHECK_FALSE(watcher.owned());
  waitMs(kBeyondGraceMs);

  CHECK_FALSE(watcher.owned());
  CHECK_FALSE(watcherServiceRegistered());
  CHECK(localWatcherObject() == nullptr);
}

TEST_CASE("disabled SNI watcher stays inactive across owner appearance and disappearance") {
  requirePrivateSessionBus();
  ensureCoreApp();

  SniWatcher watcher(false);
  for (int cycle = 0; cycle < 2; ++cycle) {
    {
      ExternalWatcher external;
      REQUIRE(external.acquire());
      waitMs(200);
      CHECK(watcherServiceRegistered());
      CHECK(watcherOwner() == external.connection().baseService());
      CHECK_FALSE(watcher.owned());
      CHECK(localWatcherObject() == nullptr);
    }

    waitMs(kBeyondGraceMs);
    CHECK_FALSE(watcher.owned());
    CHECK_FALSE(watcherServiceRegistered());
    CHECK(localWatcherObject() == nullptr);
  }
}

TEST_CASE("enabled SNI watcher claims a free name after the grace period and releases it") {
  requirePrivateSessionBus();
  ensureCoreApp();

  {
    SniWatcher watcher(true);
    CHECK_FALSE(watcher.owned());
    CHECK(localWatcherObject() == nullptr);

    waitMs(kBeyondGraceMs);
    CHECK(watcher.owned());
    CHECK(watcherServiceRegistered());
    CHECK(localWatcherObject() != nullptr);
  }

  CHECK_FALSE(watcherServiceRegistered());
  CHECK(localWatcherObject() == nullptr);
}

TEST_CASE("enabled SNI watcher does not replace an existing owner") {
  requirePrivateSessionBus();
  ensureCoreApp();

  ExternalWatcher external;
  REQUIRE(external.acquire());
  const auto owner = watcherOwner();
  REQUIRE(owner == external.connection().baseService());

  SniWatcher watcher(true);
  waitMs(kBeyondGraceMs);

  CHECK_FALSE(watcher.owned());
  CHECK(localWatcherObject() == nullptr);
  CHECK(watcherOwner() == owner);
}

// NOLINTEND(bugprone-throwing-static-initialization)
