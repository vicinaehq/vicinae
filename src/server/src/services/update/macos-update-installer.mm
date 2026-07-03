#include "macos-update-installer.hpp"
#import <Foundation/Foundation.h>
#import <Security/Security.h>
#include <QCoreApplication>
#include <QDebug>
#include <QProcess>
#include <QThreadPool>
#include <cerrno>
#include <cstring>
#include <expected>
#include <filesystem>
#include <system_error>
#include <unistd.h>

namespace fs = std::filesystem;

namespace {

constexpr const char *STAGED_PREFIX = ".Vicinae.staged.";
constexpr const char *OLD_PREFIX = ".Vicinae.old.";
constexpr int MOUNT_TIMEOUT_MS = 60000;

void trashOrRemove(const fs::path &path) {
  @autoreleasepool {
    NSFileManager *fm = [NSFileManager defaultManager];
    NSURL *url = [NSURL fileURLWithPath:@(path.c_str())];

    if (![fm trashItemAtURL:url resultingItemURL:nil error:nil]) { [fm removeItemAtURL:url error:nil]; }
  }
}

void sweepStaleArtifacts(const fs::path &installDir) {
  std::error_code ec;

  for (const auto &entry : fs::directory_iterator(installDir, ec)) {
    const std::string name = entry.path().filename().string();

    if (name.starts_with(STAGED_PREFIX) || name.starts_with(OLD_PREFIX)) {
      qInfo() << "Sweeping stale update artifact" << entry.path().c_str();
      trashOrRemove(entry.path());
    }
  }
}

std::optional<std::string> runningTeamId() {
  SecCodeRef self = nullptr;

  if (SecCodeCopySelf(kSecCSDefaultFlags, &self) != errSecSuccess) return std::nullopt;

  SecStaticCodeRef staticCode = nullptr;
  OSStatus status = SecCodeCopyStaticCode(self, kSecCSDefaultFlags, &staticCode);
  CFRelease(self);
  if (status != errSecSuccess) return std::nullopt;

  CFDictionaryRef info = nullptr;
  status = SecCodeCopySigningInformation(staticCode, kSecCSSigningInformation, &info);
  CFRelease(staticCode);
  if (status != errSecSuccess || !info) return std::nullopt;

  NSDictionary *dict = CFBridgingRelease(info);
  NSString *teamId = dict[(__bridge NSString *)kSecCodeInfoTeamIdentifier];

  if (!teamId.length) return std::nullopt;

  return std::string(teamId.UTF8String);
}

std::optional<fs::path> parseMountPoint(const QByteArray &plistData) {
  @autoreleasepool {
    NSData *data = [NSData dataWithBytes:plistData.constData() length:plistData.size()];
    NSError *error = nil;
    id plist = [NSPropertyListSerialization propertyListWithData:data
                                                          options:NSPropertyListImmutable
                                                           format:nil
                                                            error:&error];

    if (![plist isKindOfClass:[NSDictionary class]]) return std::nullopt;

    for (NSDictionary *entity in ((NSDictionary *)plist)[@"system-entities"]) {
      NSString *mountPoint = entity[@"mount-point"];
      if (mountPoint.length) return fs::path(mountPoint.UTF8String);
    }

    return std::nullopt;
  }
}

std::expected<fs::path, QString> locateBundledApp(const fs::path &mountPoint) {
  fs::path appPath;
  std::error_code ec;

  for (const auto &entry : fs::directory_iterator(mountPoint, ec)) {
    if (entry.path().extension() != ".app") continue;
    if (!appPath.empty()) return std::unexpected(QStringLiteral("Update image contains more than one app"));
    appPath = entry.path();
  }

  if (ec) return std::unexpected(QString("Failed to list update image: %1").arg(ec.message().c_str()));
  if (appPath.empty()) return std::unexpected(QStringLiteral("No app found in update image"));

  return appPath;
}

std::optional<QString> verifySignature(const fs::path &appPath, const std::string &teamId) {
  @autoreleasepool {
    NSURL *url = [NSURL fileURLWithPath:@(appPath.c_str())];
    SecStaticCodeRef code = nullptr;

    if (SecStaticCodeCreateWithPath((__bridge CFURLRef)url, kSecCSDefaultFlags, &code) != errSecSuccess) {
      return QStringLiteral("Failed to read the update's code signature");
    }

    NSString *requirementString =
        [NSString stringWithFormat:@"anchor apple generic and certificate leaf[subject.OU] = \"%s\"",
                                   teamId.c_str()];
    SecRequirementRef requirement = nullptr;

    if (SecRequirementCreateWithString((__bridge CFStringRef)requirementString, kSecCSDefaultFlags,
                                       &requirement) != errSecSuccess) {
      CFRelease(code);
      return QStringLiteral("Failed to build the signature requirement");
    }

    const SecCSFlags flags = kSecCSCheckAllArchitectures | kSecCSStrictValidate | kSecCSCheckNestedCode;
    OSStatus status = SecStaticCodeCheckValidity(code, flags, requirement);

    CFRelease(code);
    CFRelease(requirement);

    if (status != errSecSuccess) {
      return QString("Update signature verification failed (%1)").arg(status);
    }

    return std::nullopt;
  }
}

std::optional<QString> verifyBundleVersion(const fs::path &appPath, const QString &expectedVersion) {
  @autoreleasepool {
    NSBundle *bundle = [NSBundle bundleWithPath:@(appPath.c_str())];
    NSString *version = [bundle objectForInfoDictionaryKey:@"CFBundleShortVersionString"];

    if (!version.length) return QStringLiteral("Update has no CFBundleShortVersionString");

    if (expectedVersion != QString::fromNSString(version)) {
      return QString("Update version mismatch: expected %1, found %2")
          .arg(expectedVersion)
          .arg(QString::fromNSString(version));
    }

    return std::nullopt;
  }
}

} // namespace

MacosUpdateInstaller::MacosUpdateInstaller() {
  const QString appDir = QCoreApplication::applicationDirPath();

  if (!appDir.endsWith(QStringLiteral(".app/Contents/MacOS"))) return;
  if (appDir.contains(QStringLiteral("/AppTranslocation/"))) return;

  const fs::path bundlePath = fs::path(appDir.toStdString()).parent_path().parent_path();

  if (::access(bundlePath.parent_path().c_str(), W_OK) != 0) return;

  auto teamId = runningTeamId();
  if (!teamId) return;

  m_self = SelfInfo{.bundlePath = bundlePath, .teamId = std::move(*teamId)};

  sweepStaleArtifacts(bundlePath.parent_path());
}

void MacosUpdateInstaller::install(const std::filesystem::path &archive, const QString &expectedVersion) {
  if (!m_self) {
    emit failed(QStringLiteral("This installation cannot update itself"));
    return;
  }

  QThreadPool::globalInstance()->start(
      [this, archive, expectedVersion]() { performInstall(archive, expectedVersion); });
}

void MacosUpdateInstaller::performInstall(const std::filesystem::path &archive,
                                          const QString &expectedVersion) {
  emit stageChanged(QStringLiteral("Mounting update image…"));

  QProcess mount;

  mount.setProgram("/usr/bin/hdiutil");
  mount.setArguments({"attach", "-nobrowse", "-noautoopen", "-readonly", "-plist",
                      QString::fromStdString(archive.string())});
  mount.start();

  if (!mount.waitForFinished(MOUNT_TIMEOUT_MS) || mount.exitStatus() != QProcess::NormalExit ||
      mount.exitCode() != 0) {
    emit failed(QStringLiteral("Failed to mount the update image"));
    return;
  }

  const auto mountPoint = parseMountPoint(mount.readAllStandardOutput());

  if (!mountPoint) {
    emit failed(QStringLiteral("Could not find the update image mount point"));
    return;
  }

  auto failWith = [&](const QString &error) {
    detach(*mountPoint);
    emit failed(error);
  };

  emit stageChanged(QStringLiteral("Verifying update…"));

  const auto appPath = locateBundledApp(*mountPoint);
  if (!appPath) return failWith(appPath.error());

  if (auto error = verifySignature(*appPath, m_self->teamId)) return failWith(*error);
  if (auto error = verifyBundleVersion(*appPath, expectedVersion)) return failWith(*error);

  emit stageChanged(QStringLiteral("Installing update…"));

  const fs::path staged = m_self->bundlePath.parent_path() / std::format("{}{}.app", STAGED_PREFIX, getpid());

  @autoreleasepool {
    NSFileManager *fm = [NSFileManager defaultManager];

    [fm removeItemAtPath:@(staged.c_str()) error:nil];

    NSError *copyError = nil;
    if (![fm copyItemAtPath:@(appPath->c_str()) toPath:@(staged.c_str()) error:&copyError]) {
      return failWith(QString("Failed to stage update: %1")
                          .arg(QString::fromNSString(copyError.localizedDescription)));
    }
  }

  if (auto error = swapBundle(staged)) {
    std::error_code ec;
    fs::remove_all(staged, ec);
    return failWith(*error);
  }

  detach(*mountPoint);

  std::error_code ec;
  fs::remove(archive, ec);

  emit finished();
}

std::optional<QString> MacosUpdateInstaller::swapBundle(const std::filesystem::path &stagedApp) {
  const fs::path bundle = m_self->bundlePath;
  const fs::path oldPath = bundle.parent_path() / std::format("{}{}.app", OLD_PREFIX, getpid());

  if (::rename(bundle.c_str(), oldPath.c_str()) != 0) {
    return QString("Failed to move the current app aside: %1").arg(strerror(errno));
  }

  if (::rename(stagedApp.c_str(), bundle.c_str()) != 0) {
    QString error = QString("Failed to install the new app: %1").arg(strerror(errno));
    ::rename(oldPath.c_str(), bundle.c_str());
    return error;
  }

  @autoreleasepool {
    NSFileManager *fm = [NSFileManager defaultManager];
    NSURL *oldUrl = [NSURL fileURLWithPath:@(oldPath.c_str())];

    if (![fm trashItemAtURL:oldUrl resultingItemURL:nil error:nil]) {
      [fm removeItemAtURL:oldUrl error:nil];
    }
  }

  return std::nullopt;
}

void MacosUpdateInstaller::detach(const std::filesystem::path &mountPoint) {
  QProcess::startDetached("/usr/bin/hdiutil",
                          {"detach", QString::fromStdString(mountPoint.string()), "-force"});
}

void MacosUpdateInstaller::relaunch() {
  if (!m_self) {
    QCoreApplication::quit();
    return;
  }

  const QString script =
      QString("while /bin/kill -0 %1 2>/dev/null; do /bin/sleep 0.2; done; /usr/bin/open \"%2\"")
          .arg(QCoreApplication::applicationPid())
          .arg(QString::fromStdString(m_self->bundlePath.string()));

  QProcess::startDetached("/bin/sh", {"-c", script});
  QCoreApplication::quit();
}
